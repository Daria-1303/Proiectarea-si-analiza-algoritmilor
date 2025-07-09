/*
	A sports tournament organizer needs to manage elimination brackets for 64 teams.

	Each match eliminates one team, and winners advance to the next round until a champion is determined.

	The system must efficiently:

			Track which teams advance at each round

			Determine the path any team took to reach their final position

			Calculate the minimum number of matches needed

			Find all teams that were eliminated by the eventual winner

			Design an algorithm to represent this tournament structure and implement functions to answer queries about team progression and match outcomes.
*/

/*
	  Am decis sa folosim ca structura de date un arbore binar cu o implementare de tipul fiu stang si fiu drept,
	  pentru a putea urmari usor drumul parcurs de fiecare echipa in turneu.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ------------------
// Defines
// ------------------

#define DEBUG 1 // Set to 1 to enable debug messages, 0 to disable

#define MAX_TEAMS 64 // Numarul maxim de echipe


// ------------------
// Structuri de date
// ------------------

typedef struct Team {
	int id; // ID for the team
	int matches_played; // Number of matches played by the team
}Team_T;

// this is the structure for the binary tree node -> composed of subtrees representing the matches
typedef struct Match {
	Team_T* winnerTeam; // Pointer to the winning team of this match
	struct Match* left; // Pointer to the left subtree (left child)
	struct Match* right; // Pointer to the right subtree (right child)
}Match_T;

// ------------------
// Functii
// ------------------

Team_T* createTeam(int id) {
	Team_T* team = malloc(sizeof(Team_T));

	if (team == NULL) {
		perror("Memory allocation failed for team");
		exit(EXIT_FAILURE);
	}

	team->id = id;
	team->matches_played = 0; // Initialize matches played to 0

	return team;
}

void createTeams(FILE* fin, Team_T* teams, int teamCount) {

	for (int i = 0; i < teamCount; i++) {
		int id;
		if (fscanf(fin, "%d", &id) != 1) {
			perror("Error reading team ID from file\n");
			exit(EXIT_FAILURE);
		}
		teams[i] = *createTeam(id);
	}
}

void printTeam(Team_T team) {
	printf("Team ID: %d, Matches Played: %d\n", team.id, team.matches_played);
}

void printTeams(Team_T* teams, int teamCount) {
	for (int i = 0; i < teamCount; i++) {
		printTeam(teams[i]);
	}
}

Match_T* createMatch(Team_T* winnerTeam, Match_T* match1, Match_T* match2) {
	Match_T* match = malloc(sizeof(Match_T));

	if (match == NULL) {
		perror("Memory allocation failed for match");
		exit(EXIT_FAILURE);
	}

	match->winnerTeam = winnerTeam;
	match->left = match1;
	match->right = match2;

	return match;
}

Team_T* play(Team_T* team1, Team_T* team2) {
	Team_T* winner = (team1->id < team2->id) ? team1 : team2; // Example logic to determine the winner
	team1->matches_played++;
	team2->matches_played++;
	return winner;
}

void printMatch(Match_T* match) {
	if (match == NULL) {
		return;
	}
	if (match->left == NULL || match->right == NULL) {
		printf("Match with winner: Team ID %d\n", match->winnerTeam->id);
	}
	else {
		printf("Match between Team ID %d and Team ID %d, winner: Team ID %d\n",
			match->left->winnerTeam->id, match->right->winnerTeam->id, match->winnerTeam->id);
	}
}

void trackWinnersPerRound(Match_T* match) {
	if (match == NULL) {
		return;
	}
	// Post-order traversal to print matches
	trackWinnersPerRound(match->left);
	trackWinnersPerRound(match->right);
	printMatch(match);
}

void teamPath(Match_T* match, int id) {
	if (match == NULL) {
		return;
	}
	if (match->left == NULL && match->right == NULL) { // Leaf node, base case
		if (match->winnerTeam->id == id) {
			printf("Team %d reached the final match.\n", id);
		}
		return;
	}
	if (match->left != NULL && match->left->winnerTeam->id == id) {
		printf("Team %d won against Team %d\n", id, match->right->winnerTeam->id);
		teamPath(match->left, id);
	} else if (match->right != NULL && match->right->winnerTeam->id == id) {
		printf("Team %d won against Team %d\n", id, match->left->winnerTeam->id);
		teamPath(match->right, id);
	} else {
		printf("Team %d did not win in this match.\n", id);
	}
}

void teamsEliminated(Team_T* winner, Match_T* match) {
	if (match == NULL) {
		return;
	}
	if (match->left == NULL || match->right == NULL) { // Reached leaf nodes
		return;
	}
	if (match->left->winnerTeam == winner) { // If the left team won, the right team is eliminated
		teamsEliminated(winner, match->left);
		printf("Team eliminated: %d\n", match->right->winnerTeam->id);
	}
	else if (match->right->winnerTeam == winner) { // If the right team won, the left team is eliminated
		teamsEliminated(winner, match->right);
		printf("Team eliminated: %d\n", match->left->winnerTeam->id);
	}
}

//---------------------
// Main function
//---------------------

int main(int argc, char** argv) {
	if (argc < 2) {
		perror("Incorrect usage of arguments: ./exe <teams_file>\n");
		return 1;
	}
	FILE* fin = fopen(argv[1], "r");
	if (!fin) {
		perror("Error opening file\n");
		return 1;
	}
	Team_T teams[MAX_TEAMS];
	int teamCount = 0;
	if (fscanf(fin, "%d", &teamCount) != 1 || teamCount < 1 || teamCount > MAX_TEAMS) {
		perror("Invalid number of teams\n");
		fclose(fin);
		return 1;
	}
	createTeams(fin, teams, teamCount);
	if (DEBUG) {
		printf("Teams created successfully:\n");
		printTeams(teams, teamCount);
	}

	fclose(fin);

	// Create matches and simulate the tournament
	Match_T matches[MAX_TEAMS];

	for (int i = 0; i < teamCount; i++) {
		matches[i] = *createMatch(&teams[i], NULL, NULL);
	}

	int totalRounds = (int)log2(teamCount); // Calculate total rounds based on number of teams
	int matchesPerRound = teamCount / 2;

	Match_T* m = NULL;

	for (int i = 0; i < totalRounds; i++) {

		for (int j = 0; j < matchesPerRound; j++) {
			int leftIndex = j * 2;
			int rightIndex = leftIndex + 1;

			Team_T* leftTeam = matches[leftIndex].winnerTeam;
			Team_T* rightTeam = matches[rightIndex].winnerTeam;

			if (DEBUG) {
				printf("Playing match between Team %d and Team %d\n", leftTeam->id, rightTeam->id);
			}

			Team_T* winner = play(leftTeam, rightTeam);

			if (DEBUG) {
				printf("Winner of match: Team %d\n", winner->id);
			}

			// Create a new match with the winner and the two teams that played
			m = createMatch(winner, &matches[leftIndex], &matches[rightIndex]);

			matches[j] = *m; // Store the match in the matches array
		}

		// we need to update the matches per round for the next iteration
		matchesPerRound = matchesPerRound / 2;
	}
	


	printf("\nWinners per round:\n");
	trackWinnersPerRound(m);
	int idToTrack = teams[0].id; // Example ID to track
	printf("\nPath for Team ID %d:\n", idToTrack);
	teamPath(m, idToTrack);
	printf("\nTeams eliminated by the winner Team ID %d:\n", matches[0].winnerTeam->id);
	teamsEliminated(m, m);
	return 0;
}

/*

/*
	A sports tournament organizer needs to manage elimination brackets for 64 teams.

	Each match eliminates one team, and winners advance to the next round until a champion is determined.

	The system must efficiently:

			Track which teams advance at each round

			Determine the path any team took to reach their final position

			Calculate the minimum number of matches needed

			Find all teams that were eliminated by the eventual winner

			Design an algorithm to represent this tournament structure and implement functions to answer queries about team progression and match outcomes.
*/

/*
	  Am decis sa folosim ca structura de date un arbore binar cu o implementare de tipul fiu stang si fiu drept,
	  pentru a putea urmari usor drumul parcurs de fiecare echipa in turneu.
*/
/*

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ------------------
// Defines
// ------------------

#define DEBUG 1      // Set to 1 to enable debug messages, 0 to disable
#define MAX_TEAMS 64 // Numarul maxim de echipe

// ------------------
// Structuri de date
// ------------------

typedef struct Team {
	int id;               // ID for the team
	int matches_played;   // Number of matches played by the team
} Team_T;

typedef struct Match {
	Team_T* winnerTeam;   // Pointer to the winning team of this match
	struct Match* left;   // Pointer to the left subtree (left child)
	struct Match* right;  // Pointer to the right subtree (right child)
} Match_T;

// ------------------
// Functii
// ------------------

Team_T* createTeam(int id) {
	Team_T* team = malloc(sizeof(Team_T));
	if (!team) {
		perror("Memory allocation failed for team");
		exit(EXIT_FAILURE);
	}
	team->id = id;
	team->matches_played = 0;
	return team;
}

void createTeams(FILE* fin, Team_T* teams, int teamCount) {
	for (int i = 0; i < teamCount; i++) {
		int id;
		if (fscanf(fin, "%d", &id) != 1) {
			perror("Error reading team ID from file");
			exit(EXIT_FAILURE);
		}
		teams[i] = *createTeam(id);
	}
}

void printTeam(const Team_T* team) {
	printf("Team ID: %d, Matches Played: %d\n", team->id, team->matches_played);
}

void printTeams(Team_T* teams, int teamCount) {
	for (int i = 0; i < teamCount; i++) {
		printTeam(&teams[i]);
	}
}

Match_T* createMatch(Team_T* winnerTeam, Match_T* left, Match_T* right) {
	Match_T* match = malloc(sizeof(Match_T));
	if (!match) {
		perror("Memory allocation failed for match");
		exit(EXIT_FAILURE);
	}
	match->winnerTeam = winnerTeam;
	match->left = left;
	match->right = right;
	return match;
}

Team_T* play(Team_T* team1, Team_T* team2) {
	// Exemplu simplu: câștigă echipa cu ID-ul mai mic
	team1->matches_played++;
	team2->matches_played++;
	return (team1->id < team2->id) ? team1 : team2;
}

void printMatch(const Match_T* match) {
	if (!match) return;
	if (!match->left || !match->right) {
		printf("Match with winner: Team ID %d\n", match->winnerTeam->id);
	} else {
		printf("Match between Team ID %d and Team ID %d, winner: Team ID %d\n",
			match->left->winnerTeam->id,
			match->right->winnerTeam->id,
			match->winnerTeam->id);
	}
}

void trackWinnersPerRound(const Match_T* match) {
	if (!match) return;
	trackWinnersPerRound(match->left);
	trackWinnersPerRound(match->right);
	printMatch(match);
}

void teamPath(const Match_T* match, int id) {
	if (!match) return;
	// nod frunză
	if (!match->left && !match->right) {
		if (match->winnerTeam->id == id) {
			printf("Team %d reached the final match.\n", id);
		}
		return;
	}
	if (match->left && match->left->winnerTeam->id == id) {
		printf("Team %d won against Team %d\n", id, match->right->winnerTeam->id);
		teamPath(match->left, id);
	} else if (match->right && match->right->winnerTeam->id == id) {
		printf("Team %d won against Team %d\n", id, match->left->winnerTeam->id);
		teamPath(match->right, id);
	} else {
		// nu a câștigat în acest subarbore
	}
}

void teamsEliminated(const Team_T* winner, const Match_T* match) {
	if (!match || !match->left || !match->right) return;
	if (match->left->winnerTeam == winner) {
		teamsEliminated(winner, match->left);
		printf("Team eliminated: %d\n", match->right->winnerTeam->id);
	} else if (match->right->winnerTeam == winner) {
		teamsEliminated(winner, match->right);
		printf("Team eliminated: %d\n", match->left->winnerTeam->id);
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <teams_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE* fin = fopen(argv[1], "r");
	if (!fin) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}

	int teamCount;
	if (fscanf(fin, "%d", &teamCount) != 1 || teamCount < 1 || teamCount > MAX_TEAMS) {
		fprintf(stderr, "Invalid number of teams\n");
		fclose(fin);
		return EXIT_FAILURE;
	}

	Team_T teams[MAX_TEAMS];
	createTeams(fin, teams, teamCount);
	fclose(fin);

	if (DEBUG) {
		printf("Teams created successfully:\n");
		printTeams(teams, teamCount);
	}

	// Vom stoca pointerele la meciuri
	Match_T* matches[MAX_TEAMS];

	// Inițializare frunze (fiecare echipă începe ca „meci”)
	for (int i = 0; i < teamCount; i++) {
		matches[i] = createMatch(&teams[i], NULL, NULL);
	}

	int totalRounds    = (int)log2(teamCount);
	int matchesThisRnd = teamCount / 2;

	// Simulare turneu
	for (int round = 0; round < totalRounds; round++) {
		for (int j = 0; j < matchesThisRnd; j++) {
			int li = j * 2;
			int ri = li + 1;
			Team_T* L = matches[li]->winnerTeam;
			Team_T* R = matches[ri]->winnerTeam;

			if (DEBUG) {
				printf("Playing match between Team %d and Team %d\n", L->id, R->id);
			}

			Team_T* w = play(L, R);
			if (DEBUG) {
				printf("Winner of match: Team %d\n", w->id);
			}

			// Construcție nod nou
			Match_T* m = createMatch(w, matches[li], matches[ri]);
			matches[j] = m;
		}
		matchesThisRnd /= 2;
	}

	// rădăcina arborelui final
	Match_T* root = matches[0];

	printf("\nWinners per round:\n");
	trackWinnersPerRound(root);

	int idToTrack = teams[0].id; // exemplu: urmărim prima echipă
	printf("\nPath for Team ID %d:\n", idToTrack);
	teamPath(root, idToTrack);

	printf("\nTeams eliminated by the champion (Team ID %d):\n", root->winnerTeam->id);
	teamsEliminated(root->winnerTeam, root);

	return EXIT_SUCCESS;
}





*/