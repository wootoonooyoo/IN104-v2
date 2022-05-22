## Description du projet
L'objectif de ce programme est de pouvoir résoudre le jeu du morpion (Tictactoe ou oxo) avec un algorithme d'apprentissage par réenforcement, ici Q-learning. On utilisera lors de l'apprentissage la méthode epsilon-greedy pour effectuer le choix des actions.
Le programme permet d'entraîner un ordinateur étant le joueur 1 (c'est à dire le joueur commençant) ou le joueur 2, l'entraînement pourra s'effectuer entre 2 ordinateurs (COMPUTER) ou entre un ordinateur (COMPUTER) et un joueur jouant aléatoirement (RANDOM).
On pourra vérifier le bon apprentissage en jouant contre l'ordinateur ou le faisant jouer contre l'aléatoire sur une partie mais aussi sur un grand nombre de parties.
### Remarque :
- Les valeurs de reward, gamma, alpha, eps ont été fixées arbitrairement et peuvent être changées.
- L'apprentissage sur le joueur 1 est fonctionnel, il ne perd jamais à la fin de l'apprentissage (il gagne ou fait égalité au moins)
- L'apprentissage sur le joueur 2 n'est pas entièrement fonctionnel, peu importe le nombre de parties effectuées pour l'apprentissage, il subsistera toujours un cas où il perdra peu importe le nombre de parties jouées pour l'entraîner (théoriquement le jeu du morpion est fait de tel sorte que le joueur 2 ne peut jamais perdre s'il joue bien aussi)


## Fonctionnement du programme
- On exécute d'abord le programme avec un make et on le lance

- Après exécution du programme on arrive sur le menu principal nous proposant plusieurs choix :
	0. Arrêt : On arrête le programme
	1. Apprentissage : On effectue l'apprentissage, l'utilisateur aura le choix sur le nombre de parties jouées pour effectuer l'apprentissage et entre qui se fera l'apprentissage (COMPUTER vs COMPUTER ou COMPUTER vs RANDOM, RANDOM vs RANDOM n'ayant pas d'intérêt)
	2. Apprentissage parfait : On effectue un apprentissage jusqu'à qu'à que le joueur 1 ne perde jamais puis jusqu'à que le joueur 2 ne perde jamais (non fonctionnel pour joueur 2)
	3. Jeu : On effectue une partie de morpion, on a le choix entre le type de chaque joueur, vrai joueur (PLAYER), ordinateur (COMPUTER) ou aléatoire (RANDOM), chaque étape de la partie sera affichée pour suivre l'avancement
	4. Stat : On effectue un nombre de parties fixés par l'utilisateur, on choisit le type de chaque joueur et à la fin est affiché le nombre de victoires de chaque joueur et le nombre d'égalités

- Après avoir fermé le programme, on effectue le nettoyage avec make clean