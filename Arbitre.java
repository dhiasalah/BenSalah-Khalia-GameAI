import java.io.*;
import java.util.*;
import java.util.concurrent.*;

public class Arbitre {
    private static final int TIMEOUT_SECONDS = 3;
    private static final int MAX_MOVES = 400;

    public static void main(String[] args) throws Exception {
        // Process A: Friend's bot (Player 1 - odd holes)
        Process A = Runtime.getRuntime().exec(new String[]{"bot_v2.exe", "JoueurA"});
        
        // Process B: Your bot (Player 2 - even holes)
        Process B = Runtime.getRuntime().exec(new String[]{"c_version\\bot.exe", "JoueurB"});

        Joueur joueur1 = new Joueur("Player1", A, 1);
        Joueur joueur2 = new Joueur("Player2", B, 2);
 
        // Initialiser le jeu
        GameState game = new GameState();
        
        Joueur courant = joueur1;
        Joueur autre = joueur2;

        // Envoyer START aux deux joueurs
        joueur1.receive("START");
        joueur2.receive("START");

        String lastMove = "START";
        int nbCoups = 0;
        
        System.out.println("=== DEBUT DE LA PARTIE ===");
        System.out.println("Player1 (Friend's bot) vs Player2 (Your bot)");
        System.out.println();

        while (!game.isGameOver() && nbCoups < MAX_MOVES) {
            // Demander le coup au joueur courant
            long startTime = System.currentTimeMillis();
            String coup = courant.response(TIMEOUT_SECONDS);
            long endTime = System.currentTimeMillis();
            long elapsedTime = endTime - startTime;
            
            if (coup == null) {
                disqualifier(courant, autre, game, "timeout");
                break;
            }

            coup = coup.trim();
            
            // Vérifier si le bot envoie RESULT (fin de partie détectée par le bot)
            if (coup.startsWith("RESULT")) {
                System.out.println(courant.nom + " -> " + coup + " [" + elapsedTime + "ms]");
                System.out.println("\n=== FIN DE LA PARTIE ===");
                break;
            }
            
            // Vérifier si le joueur n'a pas de coup valide
            if (coup.equals("NOMOVE") || coup.equals("PASS")) {
                // Le joueur n'a plus de coups valides - fin de partie
                System.out.println(courant.nom + " -> " + coup + " (pas de coup disponible) [" + elapsedTime + "ms]");
                break;
            }
            
            System.out.println(courant.nom + " -> " + coup + " [" + elapsedTime + "ms]");
            
            // Validation et application du coup
            if (!game.applyMove(coup, courant.playerNum)) {
                disqualifier(courant, autre, game, "coup invalide: " + coup);
                break;
            }
            
            nbCoups++;
            
            // Envoyer le coup à l'adversaire
            autre.receive(coup);
            
            // Vérifier la fin du jeu
            if (game.isGameOver()) {
                break;
            }
            
            // Changement de joueur
            Joueur tmp = courant;
            courant = autre;
            autre = tmp;
        }
        
        // Afficher le résultat final
        System.out.println("\n=== RESULTAT FINAL ===");
        System.out.println("Coups joués: " + nbCoups);
        System.out.println("Score Player1: " + game.capturedSeeds[1]);
        System.out.println("Score Player2: " + game.capturedSeeds[2]);
        
        int winner = game.getWinner();
        if (winner == 1) {
            System.out.println("Gagnant: Player1");
        } else if (winner == 2) {
            System.out.println("Gagnant: Player2");
        } else {
            System.out.println("Match nul");
        }
        
        joueur1.destroy();
        joueur2.destroy();
    }

    private static void disqualifier(Joueur fautif, Joueur autre, GameState game, String raison) {
        System.out.println("RESULT " + fautif.nom + " disqualifié (" + raison + ")");
        System.out.println(autre.nom + " gagne par disqualification!");
    }

    // ===============================
    // Classe GameState - Gère l'état du jeu
    // ===============================
    static class GameState {
        int[][] holes; // [hole][color] - holes[1..16][0=R, 1=B, 2=T]
        int[] capturedSeeds; // [player] - capturedSeeds[1], capturedSeeds[2]
        
        GameState() {
            holes = new int[17][3]; // holes 1-16, couleurs R/B/T
            capturedSeeds = new int[3]; // index 1 et 2
            
            // Initialiser: 2 graines de chaque couleur par trou
            for (int i = 1; i <= 16; i++) {
                holes[i][0] = 2; // Red
                holes[i][1] = 2; // Blue
                holes[i][2] = 2; // Transparent
            }
        }
        
        boolean applyMove(String move, int player) {
            try {
                move = move.toUpperCase().trim();
                
                int hole;
                int colorIdx;
                boolean isTransparent = false;
                int transparentAs = -1;
                
                // Parser le coup: "3R", "4B", "3TR", "4TB"
                if (move.contains("T") && move.length() >= 3) {
                    // Transparent: "3TR" ou "4TB"
                    hole = Integer.parseInt(move.substring(0, move.length()-2));
                    char transChar = move.charAt(move.length()-1);
                    isTransparent = true;
                    colorIdx = 2; // Transparent
                    transparentAs = (transChar == 'R') ? 0 : 1;
                } else if (move.length() >= 2) {
                    // Normal: "3R" ou "4B"
                    hole = Integer.parseInt(move.substring(0, move.length()-1));
                    char colorChar = move.charAt(move.length()-1);
                    colorIdx = (colorChar == 'R') ? 0 : 1;
                }
                else {
                    return false;
                }
                
                // Vérifier que le trou appartient au joueur
                if (!belongsToPlayer(hole, player)) {
                    return false;
                }
                
                // Vérifier qu'il y a des graines
                if (holes[hole][colorIdx] == 0) {
                    return false;
                }
                
                int current = hole;
                int distributionColor;
                int transparentSeeds = 0;
                int coloredSeeds = 0;
                
                if (isTransparent) {
                    // Transparent: prendre les graines transparentes ET les graines de la couleur désignée
                    transparentSeeds = holes[hole][2]; // Transparent
                    coloredSeeds = holes[hole][transparentAs]; // RED ou BLUE
                    holes[hole][2] = 0;
                    holes[hole][transparentAs] = 0;
                    distributionColor = transparentAs;
                } else {
                    // Normal: prendre seulement les graines de la couleur
                    coloredSeeds = holes[hole][colorIdx];
                    holes[hole][colorIdx] = 0;
                    distributionColor = colorIdx;
                }
                
                // Distribuer d'abord les transparentes, puis les colorées
                int totalSeeds = transparentSeeds + coloredSeeds;
                int transRemaining = transparentSeeds;
                int colorRemaining = coloredSeeds;
                
                for (int i = 0; i < totalSeeds; i++) {
                    // Déterminer quelle graine distribuer (transparente d'abord)
                    int seedType;
                    if (transRemaining > 0) {
                        seedType = 2; // Transparent
                        transRemaining--;
                    } else {
                        seedType = isTransparent ? transparentAs : colorIdx;
                        colorRemaining--;
                    }
                    
                    if (distributionColor == 0) {
                        // Rouge: va dans tous les trous
                        current = (current % 16) + 1;
                        holes[current][seedType]++;
                    } else {
                        // Bleu: va uniquement dans les trous adversaires (saute par 2)
                        current = (current % 16) + 1;
                        while (belongsToPlayer(current, player)) {
                            current = (current % 16) + 1;
                        }
                        holes[current][seedType]++;
                    }
                }
                
                // Vérifier les captures
                checkCaptures(current, player);
                
                return true;
            } catch (Exception e) {
                return false;
            }
        }
        
        void checkCaptures(int lastHole, int player) {
            int current = lastHole;
            
            // La capture peut se faire depuis N'IMPORTE quel trou (y compris ses propres trous)
            // Règle: "it is allowed to take the seeds from its own hole"
            
            // Remonte en arrière en sens anti-horaire
            while (true) {
                int total = holes[current][0] + holes[current][1] + holes[current][2];
                
                // Capturer si 2 ou 3 graines
                if (total == 2 || total == 3) {
                    capturedSeeds[player] += total;
                    holes[current][0] = 0;
                    holes[current][1] = 0;
                    holes[current][2] = 0;
                    
                    // Continuer en arrière (sens anti-horaire)
                    current = current - 1;
                    if (current < 1) current = 16;
                } else {
                    break; // S'arrête dès qu'un trou n'a pas 2-3 graines
                }
            }
        }
        
        boolean belongsToPlayer(int hole, int player) {
            if (player == 1) {
                return hole % 2 == 1; // Impairs: 1,3,5,7,9,11,13,15
            } else {
                return hole % 2 == 0; // Pairs: 2,4,6,8,10,12,14,16
            }
        }
        
        boolean isGameOver() {
            // Condition 1: Un joueur a 49+ graines
            if (capturedSeeds[1] >= 49 || capturedSeeds[2] >= 49) {
                return true;
            }
            
            // Condition 2: Les deux ont 40+ (égalité)
            if (capturedSeeds[1] >= 40 && capturedSeeds[2] >= 40) {
                return true;
            }
            
            // Condition 3: Moins de 10 graines sur le plateau
            int totalOnBoard = 0;
            for (int i = 1; i <= 16; i++) {
                totalOnBoard += holes[i][0] + holes[i][1] + holes[i][2];
            }
            if (totalOnBoard < 10) {
                return true;
            }
            
            return false;
        }
        
        int getWinner() {
            // Si un joueur a 49+, il gagne
            if (capturedSeeds[1] >= 49) return 1;
            if (capturedSeeds[2] >= 49) return 2;
            
            // Sinon, compare les scores
            if (capturedSeeds[1] > capturedSeeds[2]) return 1;
            if (capturedSeeds[2] > capturedSeeds[1]) return 2;
            return 0; // Draw
        }
    }

    // ===============================
    // Classe Joueur
    // ===============================
    static class Joueur {
        String nom;
        int playerNum;
        Process process;
        BufferedWriter in;
        BufferedReader out;
        ExecutorService executor = Executors.newSingleThreadExecutor();

        Joueur(String nom, Process p, int playerNum) {
            this.nom = nom;
            this.playerNum = playerNum;
            this.process = p;
            this.in = new BufferedWriter(new OutputStreamWriter(p.getOutputStream()));
            this.out = new BufferedReader(new InputStreamReader(p.getInputStream()));
        }

        void receive(String msg) throws IOException {
            in.write(msg);
            in.newLine();
            in.flush();
        }
        
        String response(int timeoutSeconds) throws IOException {
            Future<String> future = executor.submit(() -> out.readLine());
            try {
                return future.get(timeoutSeconds, TimeUnit.SECONDS);
            } catch (TimeoutException e) {
                future.cancel(true);
                return null;
            } catch (Exception e) {
                return null;
            }
        }

        void destroy() {
            executor.shutdownNow();
            process.destroy();
        }
    }
}
