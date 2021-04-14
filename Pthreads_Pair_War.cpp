



#include <iostream>
#include <pthread.h>
#include <queue>
#include <utility>
#include <cstdio>
#include <stdlib.h>
#include <fstream>
#include <string>







//Shared

std::queue<int> deck;
//The two cards in the three players hands, indexed by thread id
std::pair <int, int> players[3];
bool game_over = false;
//turn variable keeps track of whos turn it is
int turn = 0;
int round = 0;
bool shuffled = false;
static int deck_initialize[52] = {1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,11,11,11,11,12,12,12,12,13,13,13,13};


//for the log file
std::ofstream output_file; 
std::string line; 



//Function called by dealer thread to shuffle the deck and deal the first card to each player
static void* shuffle(void* threadid) {

	int card;
	int current_round = 0;
	while(round < 3){	
		
	//empty the deck queue
		std::queue<int> empty;
		std::swap(deck, empty);

		int random_num;
		for (int i = 0; i < 52; i++) {
			random_num = i + (rand() % (52 - i));
			std::swap(deck_initialize[i], deck_initialize[random_num]);
		}

		//push contents of array to the global deck queue
		for (int i = 0; i < 52; i++)
			deck.push(deck_initialize[i]);
		
		//deals first card to each player
		card = deck.front();
		deck.pop();
		players[0].first = card;
		card = deck.front();
		deck.pop();
		players[1].first = card;
		card = deck.front();
		deck.pop();
		players[2].first = card;
		
		
		output_file << "Dealer: Shuffle" << std::endl;
		shuffled = true;
		
		//wait for round to end
		while(current_round == round){};
		current_round = round;


		shuffled = false;
	}
	return NULL;
}





//Called by players from within the game function until one wins
void player_turn(long id){

	int random_num;
	
	bool turn_complete = false;
	while(shuffled == false){}
	while(int(id) != turn){
		if(game_over == true){
			return;
		}
	}
	
	while((turn_complete == false) && (game_over == false)){
		
		if((int)id == turn){
			output_file << "Player " << id << " hand: " << players[(int)id].first << "\n";		
			players[(int)id].second = deck.front();
			output_file << "Player " << id << " draws: " << players[(int)id].second;
			output_file << std::endl;
			deck.pop();
			//check to see if player wins
			if(players[(int)id].first == players[(int)id].second){
				game_over = true;
				round++;
				output_file << "Player " << id << " hand: " << players[(int)id].first << " " << players[(int)id].second << "\n";

				output_file << "Player "<< id << " has won!" << std::endl;
				
				//players return cards to deck.
				deck.push(players[0].first);
				deck.push(players[1].first);
				deck.push(players[2].first);
				return;
			}
			else{
				//randomly select card to put back in deck
				random_num = (rand() % 2);
				if(random_num == 0){
					deck.push(players[(int)id].first);
					players[(int)id].first = players[(int)id].second;
					players[(int)id].second = 0;
				}
				else{
					deck.push(players[(int)id].second);
					players[(int)id].second = 0;
				}	
			}
			turn_complete = true;
		}
	}
	if(turn < 2)
		turn++;
	else if(turn == 2)
		turn = 0;
	


}




void *game(void *threadid)
{
   
   long tid;
   tid = (long)threadid;
   while(round < 3){
	   game_over = false;

		
		//players cycle turns until one wins
		while(game_over == false){
		if(tid < 3)
			player_turn(tid);
		}
		
			
	   if(players[(int)tid].first ==players[(int)tid].second)
		   printf("Player %ld:\n Hand: %d %d\nWin: Yes\n", tid, players[(int)tid].first, players[(int)tid].second);
	   else
		   printf("Player %ld Hand: %d \nWin: No\n", tid, players[(int)tid].first);
	   

		
	   output_file << "Player "<< tid << " has exited" << "\n";
	   
		
   }
   return NULL;
}





int main(int argc, char *argv[]){
	int seed = atoi(argv[1]);
	long threads = 4;
	pthread_t* const handle = new pthread_t [threads];
	srand(seed);
	
	
	output_file.open("log.txt", std::ios::app); 
	
   
	pthread_create(&handle[3], NULL, shuffle, (void *)(long)3);
 
   
   for(long t=0;t<3;t++){
     pthread_create(&handle[t], NULL, game, (void *)t);
     }

	for(long t=0;t<3;t++){
		pthread_join(handle[t], NULL);
	}


	output_file.close();

	delete [] handle;
    return 0;
}

