#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#define BUFSIZE 4096

enum ForFindingWordsFromCmdServer {
	FIRST_WORD=1,
	SECOND_WORD=2,
	THIRD_WORD=3,
	FOURTH_WORD=4,
	FIFTH_WORD=5,
};

struct item {
	int symb;
	struct item *next;
};

void delete_item(struct item **word_list)
{		
	struct item *word_tmp;
	while(*word_list!=NULL) {
		word_tmp=*word_list;
		*word_list=(*word_list)->next;
		delete word_tmp;
	}
}

struct words {
	char *word;
	struct words *next;
};

void delete_words(struct words **cmd_list) {
	struct words *cmd_tmp;
	while(*cmd_list!=NULL) {
		cmd_tmp=*cmd_list;
		*cmd_list=(*cmd_list)->next;
		delete [] cmd_tmp->word;
		delete cmd_tmp;
	}
}

void output_cmd_list(struct words *cmd_list) 
{
	while(cmd_list!=NULL) {
		printf("%s ",cmd_list->word);	
		cmd_list=cmd_list->next;
	}
	printf("\n");
}

int check_create_list(int symb,int flag_word)
{
	if(symb==' '||symb=='\t'||symb=='\n') {
		if(flag_word==1) {
			return 0;
		}
	}
	return -1;
}

int check_word(int symb)
{
	if(symb!=' '&&symb!='\n'&&symb!='\t') {
			return 0;	
	}
	return -1;
}

void read_word(struct item **word_list,int symb)
{
	static struct item *tmp;
	if(*word_list==NULL) {
		*word_list=new struct item;
		tmp=*word_list;
	} else {
		tmp->next=new struct item;
		tmp=tmp->next;
	}
	tmp->symb=symb;
	tmp->next=NULL;
}

int word_length(struct item *word_list)
{
	int i=0;
	while(word_list!=NULL) {
		i++;
		word_list=word_list->next;
	}
	return i;
}

char *fill_word(struct item *word_list)
{
	int k=0;
	char *word;
	word=new char [word_length(word_list)+1];
	while(word_list!=NULL) {
		word[k]=word_list->symb;
		word_list=word_list->next;
		k++;
	}
	word[k]='\0';
	return word;
}

void create_list(struct words **cmd_list,struct item *word_list)
{
	static struct words *tmp;
	if(*cmd_list==NULL) {
		*cmd_list=new struct words;
		tmp=*cmd_list;
	} else {
		tmp->next=new struct words;
		tmp=tmp->next;
	}
	tmp->word=fill_word(word_list);
	tmp->next=NULL;
}

struct words *create_cmd_list(char *buf)
{
	int i=0, symb, flag_word=0;
	struct item *word_list=NULL;
	struct words *cmd_list=NULL;
	while(buf[i]!='\0') {
		symb=buf[i];
		if(check_word(symb)==0) {
			read_word(&word_list,symb);
			flag_word=1;
		}
		if(check_create_list(symb,flag_word)==0) {
			create_list(&cmd_list,word_list);
			flag_word=0;
			delete_item(&word_list);
		}
		i++;
	}
	return cmd_list;
}

char *get_cmd_word(int c,struct words *cmd_list) 
{
	int i=1;
	while(cmd_list!=NULL) {
		if(i==c) {
			return cmd_list->word;
		}
		cmd_list=cmd_list->next;
		i++;
	}
	return NULL;
}

int find_table_character(char character, const char *table)
{
	int i=0;
	while(table[i]!=0) {
		if(table[i]==character) {
			return 0;
		}
		i++;
	}
	return -1;
}

int find_table_string(char *string, const char **table)
{
	int i=0;
	while(table[i]!=NULL) {
		if(strcmp(string,table[i])==0) {
			return 0;
		}
		i++;
	}
	return -1;
}

char *return_word(char *string)
{
	char *word;
	int i, size;
	size=strlen(string);
	word=new char [size+1];
	for(i=0;i<size;i++) {
		word[i]=string[i];
	}
	word[i]='\0';
	return word;
}

enum { 
	NOERROR,
	ERROR,
};


char create_room[]=".create";
char join_room[]=".join";
char quit_room[]="quit\n";
char quit_server[]=".quit\n";


enum {
	OVER,
	PROCESSING,
	NOTSTARTED,
	STARTED,
	NOINFO,
	INFO,
	NOEND,
	END,
	CONNECTED,
	NOTCONNECTED,
};

enum {
	QUANTITY_PARAMETERS=7,
	MSGCL_LEN=100,
	MSG_READ=100,
	BUF_SIZE=500,
};


struct game {
	int month_number;
	int players_number;
	int watchers_number;
	int info_market;
	int info_players;
	int endgame_flag;
	int server_connection;
};

struct Market {
	int raw_number;
	int prod_number;
	int min_price;
	int max_price;
};

struct players_info {
	char *nick_name;
	int player_id;
	int raw_number;
	int prod_number;
	int money_number;
	int fact_number;
	int sold_raw_number;
	int sold_price_number;
	int bought_prod_number;
	int bought_price_number;
	int auction_month;
	struct players_info *next;
};

class PlayersInfo {
	struct players_info *players_list;
public:
	PlayersInfo() {
		players_list=NULL;
	}
	~PlayersInfo() {
		struct players_info *players_tmp;
		while(players_list!=NULL) {
			players_tmp=players_list;
			players_list=players_list->next;
			delete players_tmp;
		}
	}
	players_info *find_player_name_in_list(char *name);
	players_info *find_player_id_in_list(int player_id);
	int get_player_id(char *nick_name);
	int get_players_number() {
		int i=0;
		struct players_info *players_tmp=players_list;
		while(players_tmp!=NULL) {
			i++;
			players_tmp=players_tmp->next;
		}
		return i;
	}
	void info_players_list(struct words *cmd_list);
	void sold_players_list(struct words *cmd_list);
	void bought_players_list(struct words *cmd_list);
	void delete_player_from_list(char *name);
	void increase_auction_month() {
		struct players_info *players_tmp=players_list;
		while(players_tmp!=NULL) {
			players_tmp->auction_month=
				players_tmp->auction_month+1;
			players_tmp=players_tmp->next;
		}
	}
	void output_players_info();

private:
	void add_data_players_list(struct players_info *players_tmp,
						struct words *cmd_list);
	void set_nick_name_and_player_id(struct words *cmd_list);
	void zero_auction_information(struct players_info *players_tmp);
	void set_init_data(struct players_info *players_tmp,
						struct words *cmd_list);
	void add_new_player_in_list(struct words *cmd_list);
};

void PlayersInfo::output_players_info()
{
	struct players_info *players_tmp=players_list;
	while(players_tmp!=NULL) {
		printf("====%s====\n",players_tmp->nick_name);
		printf("Player_id=%d\n",players_tmp->player_id);
		printf("Raw=%d\n",players_tmp->raw_number);
		printf("Prod=%d\n",players_tmp->prod_number);
		printf("Money=%d\n",players_tmp->money_number);
		printf("Factories=%d\n",players_tmp->fact_number);
		printf("Auction raw buy=%d\n",players_tmp->sold_raw_number);
		printf("Auction raw price buy=%d\n",
					players_tmp->sold_price_number);
		printf("Auction prod sell=%d\n",
				players_tmp->bought_prod_number);
		printf("Auction prod price sell=%d\n",
				players_tmp->bought_price_number);
		printf("============\n");
		players_tmp=players_tmp->next;
	}
}

void PlayersInfo::delete_player_from_list(char *name) 
{
	struct players_info **players_tmp=&players_list;
	while(*players_tmp!=NULL) {
		if(strcmp((*players_tmp)->nick_name,name)==0) {
			struct players_info *tmp=*players_tmp;
			*players_tmp=(*players_tmp)->next;
			delete tmp;
		} else {
			players_tmp=&(*players_tmp)->next;
		}
	}
}

void PlayersInfo::zero_auction_information(struct players_info *players_tmp)
{
	players_tmp->sold_raw_number=0;
	players_tmp->bought_prod_number=0;
	players_tmp->sold_price_number=0;
	players_tmp->bought_price_number=0;
	players_tmp->auction_month=0;
}

void PlayersInfo::add_data_players_list(struct players_info *players_tmp,
						struct words *cmd_list) 
{
	players_tmp->raw_number=atoi(get_cmd_word(SECOND_WORD,cmd_list));
	players_tmp->prod_number=atoi(get_cmd_word(THIRD_WORD,cmd_list));
	players_tmp->money_number=atoi(get_cmd_word(FOURTH_WORD,cmd_list));
	players_tmp->fact_number=atoi(get_cmd_word(FIFTH_WORD,cmd_list));
	if(players_tmp->auction_month>0) {
		zero_auction_information(players_tmp);
	}

}

void PlayersInfo::set_init_data(struct players_info *players_tmp,
						struct words *cmd_list)
{
	players_tmp->nick_name=
		new char [strlen(get_cmd_word(FIRST_WORD,cmd_list))];
	players_tmp->nick_name=
		return_word(get_cmd_word(FIRST_WORD,cmd_list));
	zero_auction_information(players_tmp);
}

players_info* PlayersInfo::find_player_name_in_list(char *name) {
	struct players_info *players_tmp=players_list;
	while(players_tmp!=NULL) {
		if(strcmp(players_tmp->nick_name,name)==0) {
				return players_tmp;
		}
		players_tmp=players_tmp->next;
	}
	return NULL;
}

players_info* PlayersInfo::find_player_id_in_list(int player_id)
{
	struct players_info *players_tmp=players_list;
	while(players_tmp!=NULL) {
		if(players_tmp->player_id==player_id) {
			return players_tmp;
		}
		players_tmp=players_tmp->next;
	}
	return NULL;
}

int PlayersInfo::get_player_id(char *nick_name)
{
	struct players_info *players_tmp=players_list;
	if((players_tmp=find_player_name_in_list(nick_name))!=NULL) {
		return players_tmp->player_id;
	} else {
		printf("Your name was not found\n");
		return 0;
	}
}

void PlayersInfo::add_new_player_in_list(struct words *cmd_list) 
{
	int player_id=1;
	struct players_info *players_tmp;
	if(players_list==NULL) {
		players_list=new struct players_info;
		players_tmp=players_list;
	} else {
		players_tmp=players_list;
		while(players_tmp->next!=NULL) {
			player_id++;
			players_tmp=players_tmp->next;
		}
		player_id++;
		players_tmp->next=new struct players_info;
		players_tmp=players_tmp->next;
	}
	players_tmp->player_id=player_id;
	set_init_data(players_tmp,cmd_list);
	add_data_players_list(players_tmp,cmd_list);
	players_tmp->next=NULL;
}

void PlayersInfo::info_players_list(struct words *cmd_list)
{
	struct players_info *players_tmp=players_list;
	if((players_tmp=find_player_name_in_list(get_cmd_word(
					FIRST_WORD,cmd_list)))==NULL) {
		add_new_player_in_list(cmd_list);
	} else {
		add_data_players_list(players_tmp,cmd_list);
	}
}

void PlayersInfo::sold_players_list(struct words *cmd_list)
{
	struct players_info *players_tmp=players_list;
	if((players_tmp=find_player_name_in_list(get_cmd_word(
					FIRST_WORD,cmd_list)))!=NULL) {
		players_tmp->sold_raw_number=
			atoi(get_cmd_word(SECOND_WORD,cmd_list));
		players_tmp->sold_price_number=
			atoi(get_cmd_word(THIRD_WORD,cmd_list));
		players_tmp->auction_month=0;
	} 
}

void PlayersInfo::bought_players_list(struct words *cmd_list)
{
	struct players_info *players_tmp=players_list;
	if((players_tmp=find_player_name_in_list(get_cmd_word(
					FIRST_WORD,cmd_list)))!=NULL) {
		players_tmp->bought_prod_number=
			atoi(get_cmd_word(SECOND_WORD,cmd_list));
		players_tmp->bought_price_number=
			atoi(get_cmd_word(THIRD_WORD,cmd_list));
		players_tmp->auction_month=0;
	} 
}


class Server {
	int sock;
	char *nick_name;
	char *action;
	int quantity_players;
	int max_players;
	char buffer[BUF_SIZE];
	int buffer_base;
	struct game game_info;
	PlayersInfo players;
	struct Market market_info;

	void create_str_join(char **argv);
	void processing_parameters(char **argv);
	void set_start_game_info();
	
	void delete_players_info();
	void create_players_list(struct words *cmd_list);
	void get_all_info(); 
	void get_bot_info(struct words *cmd_list);
	void get_market(struct words *cmd_list);
	void analyse_msg(struct words *cmd_list);
	void accept_players(struct words *cmd_list);
	void check_string();
	void create_string(int quantity_byte, char *message);
	void msg_from_server();

	char *getip_addr(char *ip_addr, char *zero_ip_addr); 
	void send_init_data();
	int get_random_number();
	

public:
	void make_game_name_zero(char *action);
	void make_game_name_one(char *action,int first_param);
	void make_game_name_two(char *action,int first_param,int second_param);
	int check_func_without_param(char *function_name);
	int check_func_with_param(char *function_name,int player_id);

	Server(int argc, char **argv); 
	int work_with_init_data(char **argv);
	bool check_end_game();
	void work_with_socket();
	void get_number_players_watchers(struct words *cmd_list);
	void start_of_turn();
	int check_all_info();

};




int check_bot_action(char *string)
{
	if(strcmp(string,create_room)==0) {
		return 0;
	} 
	if(strcmp(string,join_room)==0) {
		return 0;
	} 
	return -1;
}

void check_parameters(int argc, char **argv)
{
	if(argc!=QUANTITY_PARAMETERS) {
		printf("Bad number of parameters\n");
		exit(1);
	}
	if(check_bot_action(argv[4])==-1) {
		printf("Bad action of bot, see the argument number 4\n");
		exit(1);
	}
}

void Server::create_str_join(char **argv)
{
	int size=strlen(argv[4])+strlen(argv[5]);
	action=new char [size+3];
	int i;
	int j=0;
	for(i=0;i<(int)strlen(argv[4]);i++) {
		action[i]=argv[4][i];
		putchar(argv[4][i]);
	}
	action[i]=' ';
	for(i=strlen(argv[4])+1;i<size+1;i++) {
		action[i]=argv[5][j];
		j++;
	}
	action[i]='\n';
	i++;
	action[i]='\0';
}

void Server::set_start_game_info()
{
	quantity_players=0;
	buffer_base=0;
	game_info.server_connection=CONNECTED;
	game_info.endgame_flag=NOEND;
	game_info.month_number=1;
	game_info.info_market=NOINFO;
	game_info.info_players=NOINFO;
	game_info.players_number=0;
	game_info.watchers_number=0;
}



void Server::processing_parameters(char **argv)
{
	nick_name=new char [strlen(argv[3])+1];
	strcpy(nick_name,argv[3]);
	nick_name[strlen(argv[3])]='\0';
	if(strcmp(join_room,argv[4])==0) {
		create_str_join(argv);
	} else {
		action=new char [strlen(argv[4])+2];
		strcpy(action,argv[4]);
		action[strlen(argv[4])]='\n';
		action[strlen(argv[4])+1]='\0';
		max_players=atoi(argv[5]);
	}
}

Server::Server(int argc, char **argv) 
{
	processing_parameters(argv);
	set_start_game_info();
}

void Server::get_all_info() 
{
	char market_str[]="market\n";
	char info_str[]="info\n";
	write(sock,market_str,strlen(market_str));
	write(sock,info_str,strlen(info_str));
}

void Server::get_market(struct words *cmd_list)
{
	market_info.raw_number=atoi(get_cmd_word(FIRST_WORD,cmd_list));
	market_info.min_price=atoi(get_cmd_word(SECOND_WORD,cmd_list));
	market_info.prod_number=atoi(get_cmd_word(THIRD_WORD,cmd_list));
	market_info.max_price=atoi(get_cmd_word(FOURTH_WORD,cmd_list));
}	
	
void Server::get_number_players_watchers(struct words *cmd_list)
{
	game_info.players_number=atoi(get_cmd_word(FIRST_WORD,cmd_list));
	game_info.watchers_number=atoi(get_cmd_word(THIRD_WORD,cmd_list));
}

void Server::analyse_msg(struct words *cmd_list)
{
	if(strcmp(cmd_list->word,"START")==0) {
		get_all_info();
	} else 
	if(strcmp(cmd_list->word,"ENDTURN")==0) {
		get_all_info();
		game_info.month_number++;
		players.increase_auction_month();
		players.output_players_info();
	} else 
	if(strcmp(cmd_list->word,"PLAYERS")==0) {
		get_number_players_watchers(cmd_list->next);
		game_info.info_players=INFO;
	} else 
	if(strcmp(cmd_list->word,"MARKET")==0) {
		get_market(cmd_list->next);
		game_info.info_market=INFO;
	} else 
	if(strcmp(cmd_list->word,"INFO")==0) {
		players.info_players_list(cmd_list->next);
	} else
	if(strcmp(cmd_list->word,"SOLD")==0) {
		players.sold_players_list(cmd_list->next);
	} else
	if(strcmp(cmd_list->word,"BOUGHT")==0) {
		players.bought_players_list(cmd_list->next);
	} else
	if(strcmp(cmd_list->word,"WINNER")==0||
	strcmp(cmd_list->word,"YOU_WIN")==0) {
		write(sock,quit_room,strlen(quit_room));
		write(sock,quit_server,strlen(quit_server));
		game_info.endgame_flag=END;
	} else
	if(strcmp(cmd_list->word,"BANKRUPT")==0) {
		players.delete_player_from_list(cmd_list->next->word);
		if(strcmp(cmd_list->next->word,nick_name)==0) {
			write(sock,quit_room,strlen(quit_room));
			write(sock,quit_server,strlen(quit_server));
			game_info.endgame_flag=END;
		}
	}
}

void Server::accept_players(struct words *cmd_list)
{
	if(strcmp(cmd_list->next->word,"JOIN\0")==0) {
		quantity_players++;
		printf("Quantity players=%d\n",quantity_players);
	}
	if(quantity_players==max_players) {
		char start_str[]="start\n";
		write(sock,start_str,strlen(start_str));
	}
}


void Server::check_string()
{
	struct words *cmd_list=create_cmd_list(buffer);
	if(strcmp(cmd_list->word,"#")!=0) {
		if(strcmp(cmd_list->word,"&")==0) { 
			analyse_msg(cmd_list->next);
		}	
		if(strcmp(cmd_list->word,"@+")==0) {
			if(strcmp(action,".create\n")==0) {	
				accept_players(cmd_list);
			}
		}
	}
	delete_words(&cmd_list);
	return;
}

void Server::create_string(int quantity_byte, char *message)
{
	int i;
	for(i=0;i<quantity_byte;i++) {
		putchar(message[i]);
		if(message[i]!='\0'&&message[i]!='\r') {
			if(message[i]!='\n') {
				buffer[buffer_base]=message[i];
				buffer_base++;
			} else {
				buffer[buffer_base]='\n';
				buffer_base++;
				buffer[buffer_base]='\0';
				check_string();
				buffer_base=0;
			}
		}
	}
}

void Server::work_with_socket()
{
	int quantity_byte;
	char message[MSG_READ+1];
	quantity_byte=read(sock,message,MSG_READ);
	if(quantity_byte!=0) {
		message[quantity_byte]='\0';
		create_string(quantity_byte,message);
	} else {
		game_info.server_connection=NOTCONNECTED;
	}
}


void Server::make_game_name_zero(char *action) 
{
	if(strcmp(action,"endturn")==0) {
		char turn_str[]="turn\n";
		write(sock,turn_str,strlen(turn_str));
	}
}

void Server::make_game_name_one(char *action,int first_param) 
{
	if(strcmp(action,"prod")==0) {
		char prod_str[100];
		sprintf(prod_str,"prod %d\n",first_param);
		write(sock,prod_str,strlen(prod_str));
	} else
	if(strcmp(action,"build")==0) {
		char build_str[100];
		sprintf(build_str,"build %d\n",first_param);
		write(sock,build_str,strlen(build_str));
	}
}

void Server::make_game_name_two(char *action, int first_param, 
							int second_param) 
{
	if(strcmp(action,"sell")==0) {
		char sell_str[100];
		sprintf(sell_str,"sell %d %d\n",first_param,second_param);
		write(sock,sell_str,strlen(sell_str));
	} else
	if(strcmp(action,"buy")==0) {
		char buy_str[100];
		sprintf(buy_str,"buy %d %d\n",first_param,second_param);
		write(sock,buy_str,strlen(buy_str));
	}
}

const int level_change[3]={6,4,2};


int Server::get_random_number()
{
	int r, sum=0, new_level=0; 
	static int number=0;
	srand(getpid());
	r=1+(int)(12.0*rand()/(RAND_MAX+1.0));
	while(r>sum) {
		sum=sum+level_change[new_level];
		new_level++;
	}
	if(new_level==1) {
		number=rand()%50+1;
	} else
	if(new_level==2) {
		number=rand()%50+50;
	} else
	if(new_level==3) {
		number=rand()%100+100;
	}
	printf("======================================\n"
		"\n\n\n\nrandom_number=%d\n\n\n\n\n",number);
	return number;
}

int Server::check_func_without_param(char *function_name)
{
	if(strcmp(function_name,"get_random_number")==0) {
		return get_random_number();
	} else
	if(strcmp(function_name,"my_id")==0) {
		return players.get_player_id(nick_name);
	} else 
	if(strcmp(function_name,"turn")==0) {
		return game_info.month_number;
	} else 
	if(strcmp(function_name,"players")==0) {
		return game_info.players_number+game_info.watchers_number;
	} else 
	if(strcmp(function_name,"active_players")==0) {
		return game_info.players_number;
	} else 
	if(strcmp(function_name,"supply")==0) {
		return market_info.raw_number;
	} else 
	if(strcmp(function_name,"raw_price")==0) {
		return market_info.min_price;
	} else 
	if(strcmp(function_name,"demand")==0) {
		return market_info.prod_number;
	} else 
	if(strcmp(function_name,"production_price")==0) {
		return market_info.max_price;
	}
	return 0;
}




int Server::check_func_with_param(char *function_name,int player_id)
{
	struct players_info *players_tmp=NULL;
	players_tmp=players.find_player_id_in_list(player_id);
	if(strcmp(function_name,"check_player")==0) {
		if(players_tmp!=NULL) {
			return 1;
		} else {
			return 0;
		}
	} else
	if(players_tmp!=NULL) {
		if(strcmp(function_name,"money")==0) {
			return players_tmp->money_number;
		} else  
		if(strcmp(function_name,"raw")==0) {
			return players_tmp->raw_number;
		} else  
		if(strcmp(function_name,"production")==0) {
			return players_tmp->prod_number;
		} else  
		if(strcmp(function_name,"factories")==0) {
			return players_tmp->fact_number;
		} else  
		if(strcmp(function_name,"result_sold_raw")==0) {
			return players_tmp->sold_raw_number;
		} else  
		if(strcmp(function_name,"result_raw_price")==0) {
			return players_tmp->sold_price_number;
		} else  
		if(strcmp(function_name,"result_bought_prod")==0) {
			return players_tmp->bought_prod_number;
		} else  
		if(strcmp(function_name,"result_prod_price")==0) {
			return players_tmp->bought_price_number;
		}
	} 
	return 0;
}

int Server::check_all_info() 
{
	if(game_info.info_market==INFO) {
		if(game_info.players_number==players.get_players_number()) {
			if(game_info.info_players==INFO) {
				//players.output_players_info();
				return INFO;
			} else {
				return NOINFO;
			}
		} else {
			return NOINFO;
		}
	} else {
		return NOINFO;
	}
}

void Server::start_of_turn() 
{
	game_info.info_market=NOINFO;
	game_info.info_players=NOINFO;
}

bool Server::check_end_game()
{
	if(game_info.endgame_flag==END) {
		printf("The game is over\n");
		return true;
	} else 
	if(game_info.server_connection==NOTCONNECTED) {
		printf("The connection with server failed\n");
		return true;
	}
	return false;
}

void Server::send_init_data()
{
	char send_nick_name[30];
	char chat_off[]="chat off\n";
	sprintf(send_nick_name,"%s\n",nick_name);
	write(sock,send_nick_name,strlen(send_nick_name));
	write(sock,action,strlen(action));
	write(sock,chat_off,strlen(chat_off));
}

char* Server::getip_addr(char *ip_addr, char *zero_ip_addr) 
{
	if(strlen(ip_addr)==1&&ip_addr[0]=='0') {
		return zero_ip_addr;
	}
	return ip_addr;
}

int Server::work_with_init_data(char **argv)
{
	sock=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in addr;
	char zero_ip_addr[]="0.0.0.0";
	addr.sin_family=AF_INET;
	addr.sin_port=htons(atoi(argv[2]));
	if(!inet_aton(getip_addr(argv[1],zero_ip_addr),&addr.sin_addr)) {
		perror("inet_aton");
		return ERROR;
	}
	if(0!=connect(sock,(struct sockaddr*)&addr,sizeof(addr))) {
		perror("connect");
		return ERROR;
	}
	send_init_data();
	return NOERROR;
}








//============================FILEANALYSE=============================//
enum lexeme_type {
	NOTYPE,
	NUMBER,
	STRINGCONST,
	LABEL,
	FUNCTION,
	VARIABLE,
	KEYWORD,
	DELIMITER,
};

void output_char_list(struct item *char_list)
{
	while(char_list!=NULL) {
		putchar(char_list->symb);
		char_list=char_list->next;
	}
}

struct lexemes {
	char *word;
	int type;
	int string_number;
	struct lexemes *next; 
};

void delete_lexeme_list(struct lexemes **lex_list)
{
	while((*lex_list)!=NULL) {
		struct lexemes *lex_tmp=*lex_list;
		*lex_list=(*lex_list)->next;
		delete lex_tmp;
	}
}

struct errors {
	int number;
	int string_number;
	char *word;
};


FILE* open_file(char **argv)
{
	FILE* fd=fopen(argv[6],"r");
	if(fd==NULL) {
		perror(argv[6]);
		exit(1);
	}
	return fd;
}

const char *func_without_param[]={"my_id", "turn", "players", 
		"active_players", "supply", "raw_price", 
		"demand", "production_price", "get_random_number", NULL};

const char *func_with_param[]={"money", "raw", "production", 
		"factories", "result_sold_raw",
		"result_raw_price", "result_bought_prod",
		"result_prod_price","check_player", NULL};

class Lexical_Analysis {

	static const char *key_table[];
	static const char delim_table[];

	enum states {
		NUM,
		STRING,
		FLV,
		KEY,
		DELIM,
		INIT,
	};

	enum errortypes {
		NOERROR,
		ERROR,
		KEYWORD_ERR,
		WRONGSYMB_ERR,
		WRONGSTATE_ERR,
		NAME_FUNC_ERR,
	};


	int lex_type;
	int state;
	struct errors error;
	int string_number;
	struct item *char_list;
	struct lexemes *lexeme;

	void fill_error(int error_number, int str_number, char *word);
	void delete_char_list();
	void create_char_list(char symb);
	int get_item_len(struct item *char_list);
	void create_lexeme();
	void add_lexeme(char symb);
	struct lexemes* check_ready_or_add_lexeme(char symb);
	struct lexemes* number_state(char symb);
	struct lexemes* string_state(char symb);
	struct lexemes* label_func_var_states(char symb);
	struct lexemes* key_state(char symb);
	struct lexemes* delim_state(char symb);
	struct lexemes* init_state(char symb);


	void analyse_error();
public:
	Lexical_Analysis();
	/*Func in feed_character*/
	struct lexemes* feed_character(char symb);	
	int check_lexeme_list(struct lexemes *lex_list);
	int check_error_state();

};

const char* Lexical_Analysis::key_table[]={"if","then","goto","print",
			"buy","sell","prod","build","endturn",NULL};

 
const char Lexical_Analysis::delim_table[]={'+','-','*','/','%','<','>',
				':',',',';','(',')','=','[',']','!','{','}',
				'|','&'};

Lexical_Analysis::Lexical_Analysis()
{
	lex_type=NOTYPE;
	error.number=NOERROR;
	error.word=NULL;
	string_number=1;
	state=INIT;
	char_list=NULL;
	lexeme=NULL;
}

void Lexical_Analysis::fill_error(int error_number, int str_number, 
								char *word)
{
	error.number=error_number;
	error.string_number=str_number;
	error.word=word;
}

void Lexical_Analysis::delete_char_list()
{
	struct item *char_tmp;
	while(char_list!=NULL) {
		char_tmp=char_list;
		char_list=char_list->next;
		delete char_tmp;
	}
}

void Lexical_Analysis::create_char_list(char symb)
{
	static struct item *char_tmp;
	if(char_list==NULL) {
		char_list=new struct item;
		char_tmp=char_list;
	} else {
		char_tmp->next=new struct item;
		char_tmp=char_tmp->next;
	}
	char_tmp->symb=symb;
	char_tmp->next=NULL;
}

int Lexical_Analysis::get_item_len(struct item *char_list)
{
	int i=0;
	while(char_list!=NULL) {
		i++;
		char_list=char_list->next;
	}
	return i;
}

void Lexical_Analysis::create_lexeme()
{
	lexeme=new struct lexemes;
	lexeme->type=lex_type;
	lexeme->string_number=string_number;
	lexeme->word=new char [get_item_len(char_list)+1];
	int i=0;
	struct item *char_tmp=char_list;
	while(char_tmp!=NULL) {
		lexeme->word[i]=char_tmp->symb;
		i++;
		char_tmp=char_tmp->next;
	}
	lexeme->word[i]='\0';
	lexeme->next=NULL;
}

void Lexical_Analysis::add_lexeme(char symb)
{
	struct lexemes *lexeme_tmp=lexeme;
	lexeme_tmp->next=new struct lexemes;
	lexeme_tmp=lexeme_tmp->next;
	lexeme_tmp->type=lex_type;
	lexeme_tmp->string_number=string_number;
	lexeme_tmp->word=new char [2];
	lexeme_tmp->word[0]=symb;
	lexeme_tmp->word[1]='\0';
	lexeme_tmp->next=NULL;
}
		
struct lexemes* Lexical_Analysis::check_ready_or_add_lexeme(char symb)
{
	if(symb==' '||symb=='\n'||symb=='\t') {
		create_lexeme();
		delete_char_list();
		state=INIT;
		lex_type=NOTYPE;
		if(symb=='\n') {
			string_number++;
		}
		return lexeme;
	}
	if(find_table_character(symb,delim_table)==0) {
		create_lexeme();
		delete_char_list();
		state=DELIM;
		lex_type=DELIMITER;
		add_lexeme(symb);
		state=INIT;
		lex_type=NOTYPE;
		return lexeme;
	}
	return NULL;
}

struct lexemes* Lexical_Analysis::number_state(char symb)
{
	if(symb>='0'&&symb<='9') {
		create_char_list(symb);
		return NULL;
	}
	if(check_ready_or_add_lexeme(symb)!=NULL) {
		return lexeme;
	}
	fill_error(WRONGSYMB_ERR,string_number,NULL);
	return NULL;
}

struct lexemes* Lexical_Analysis::key_state(char symb)
{
	if((symb>='a'&&symb<='z')||
	(symb>='A'&&symb<='Z')) {
		create_char_list(symb);
		return NULL;
	}
	if(check_ready_or_add_lexeme(symb)!=NULL) {
		return lexeme;
	}
	fill_error(WRONGSYMB_ERR,string_number,NULL);
	return NULL;
}

struct lexemes* Lexical_Analysis::label_func_var_states(char symb)
{
	if((symb>='0'&&symb<='9')||
		(symb>='a'&&symb<='z')||
		(symb>='A'&&symb<='Z')
		||symb=='_') {
			create_char_list(symb);
		return NULL;
	}
	if(check_ready_or_add_lexeme(symb)!=NULL) {
		return lexeme;
	}
	fill_error(WRONGSYMB_ERR,string_number,NULL);
	return NULL;
}

struct lexemes* Lexical_Analysis::string_state(char symb)
{
	if(symb!='"') {
		create_char_list(symb);
		return NULL;
	} else {
		create_lexeme();
		delete_char_list();
		state=INIT;
		lex_type=NOTYPE;
		return lexeme;
	}
}

struct lexemes* Lexical_Analysis::delim_state(char symb)
{
	create_char_list(symb);
	create_lexeme();
	delete_char_list();
	state=INIT;
	lex_type=NOTYPE;
	return lexeme;
}

struct lexemes* Lexical_Analysis::init_state(char symb) 
{
	if(symb>='0'&&symb<='9') {
		create_char_list(symb);
		state=NUM;
		lex_type=NUMBER;
		return NULL;
	}
	if(symb=='"') {
		state=STRING;
		lex_type=STRINGCONST;
		return NULL;
	}
	if(symb=='?') {
		state=FLV;
		lex_type=FUNCTION;
		return NULL;
	}
	if(symb=='@') {	
		state=FLV;
		lex_type=LABEL;
		return NULL;
	}
	if(symb=='$') {
		state=FLV;
		lex_type=VARIABLE;
		return NULL;
	}
	if((symb>='a'&&symb<='z')||
	(symb>='A'&&symb<='Z')) {
		create_char_list(symb);
		state=KEY;
		lex_type=KEYWORD;
		return NULL;
	}
	if(symb==' '||symb=='\t'||symb=='\n') {
		if(symb=='\n') {
			string_number++;
		}
		return NULL;
	}
	if(find_table_character(symb,delim_table)==0) {
		create_char_list(symb);
		state=DELIM;
		lex_type=DELIMITER;
		create_lexeme();
		delete_char_list();
		state=INIT;
		lex_type=NOTYPE;
		return lexeme;
	}
	fill_error(WRONGSYMB_ERR,string_number,NULL);
	return NULL;
}

struct lexemes* Lexical_Analysis::feed_character(char symb)
{
	switch (state) {
		case NUM:
			return number_state(symb);
		case STRING:
			return string_state(symb);
		case FLV:
			return label_func_var_states(symb);
		case KEY:
			return key_state(symb);
		case DELIM:
			return delim_state(symb);
		case INIT:
			return init_state(symb);
		default:
			fill_error(WRONGSYMB_ERR,string_number,NULL);
			return NULL;
	}
}


int Lexical_Analysis::check_lexeme_list(struct lexemes *lex_list)
{
	while(lex_list!=NULL) {
		if(lex_list->type==KEYWORD) {
			if(find_table_string(lex_list->word,key_table)!=0) {
				fill_error(KEYWORD_ERR,lex_list->string_number,
						return_word(lex_list->word));
				return ERROR;
			}
		} else
		if(lex_list->type==FUNCTION) {	
			if(find_table_string(lex_list->word,
						func_with_param)!=0) {
				if(find_table_string(lex_list->word,
						func_without_param)!=0) {

					fill_error(NAME_FUNC_ERR,
						lex_list->string_number,
						return_word(lex_list->word));
					return ERROR;
				}
			}
		}
		lex_list=lex_list->next;
	}
	return NOERROR;
}


void Lexical_Analysis::analyse_error()
{
	if(error.number==WRONGSYMB_ERR) {
		fprintf(stderr,
			"Error in text has occured in string number < %d >\n",
						error.string_number);
	} else
	if(error.number==KEYWORD_ERR) {
		fprintf(stderr,
			"Error has occured in the key word "
			"< %s > in string number < %d >\n",
				error.word, error.string_number);
	} else
	if(error.number==WRONGSTATE_ERR) {
		fprintf(stderr,
			"Error has occured in the state "
			"in string number < %d >\n",
					error.string_number);
	} else
	if(error.number==NAME_FUNC_ERR) {
		fprintf(stderr,
			"Error has occured in the function "
			"< %s > in string number < %d >, there is no such "
			"function name in the list\n",
					error.word, error.string_number);
	}
}

int Lexical_Analysis::check_error_state()
{
	if(error.number!=NOERROR) {
		analyse_error();
		return ERROR;
	} 
	return NOERROR;
}

void create_lexeme_list(struct lexemes *lexeme, struct lexemes **lex_list)
{
	static struct lexemes *lex_tmp;
	if(*lex_list==NULL) {
		*lex_list=lexeme;
		lex_tmp=*lex_list;
	} else {
		lex_tmp->next=lexeme;
	}
	while(lex_tmp->next!=NULL) {
		lex_tmp=lex_tmp->next;
	}

}

void output_lexeme_list(struct lexemes *lex_list)
{
	printf("================Lexeme list=============\n");
	while(lex_list!=NULL) {
		printf("%s",lex_list->word);
		if(lex_list->type==NUMBER) {
			printf("_____NUMBER");
		}
		if(lex_list->type==STRINGCONST) {
			printf("_____STRING CONSTANT");
		}
		if(lex_list->type==LABEL) {
			printf("_____LABEL");
		}
		if(lex_list->type==VARIABLE) {
			printf("_____VARIABLE");
		}
		if(lex_list->type==FUNCTION) {
			printf("_____FUNCTION");
		}
		if(lex_list->type==KEYWORD) {
			printf("_____KEY WORD");
		}
		if(lex_list->type==DELIMITER) {
			printf("_____DELIMITER");
		}
		printf("____string number = %d\n",
				lex_list->string_number);
		lex_list=lex_list->next;
	}
	printf("========================================\n");
}

struct lexemes *lexical_analysis(FILE* fd)
{
	Lexical_Analysis LA;
	struct lexemes *lexeme_list=NULL;
	int symb;
	while((symb=fgetc(fd))!=EOF) {
		struct lexemes *lex=LA.feed_character(symb);
		if(lex!=NULL) {
			create_lexeme_list(lex,&lexeme_list);
		} else {
			if(LA.check_error_state()==ERROR) {
				exit(1);
			}
		}
	}
	if(LA.check_lexeme_list(lexeme_list)==ERROR) {
		LA.check_error_state();
		exit(1);
	}
	return lexeme_list;
}










enum ParsingErrors {
	STATEMENT_ERR,
	SEMICOLON_ERR,
	GAME_NAME_ERR,
	GAME_OPERATOR_ERR,
	KEY_WORD_ERR,
	NULL_LIST_ERR,
	PRINT_ERR,
	OPERAND_ERR,
	SQUARE_BRACKET_ERR,
	DOUBLE_EQUAL_SIGN_ERR,
	EQUAL_SIGN_ERR,
	IF_ERR,
	THEN_ERR,
	LABEL_ERR,
	GOTO_ERR,
	VARIABLE_ERR,
	DELIMITER_ERR,
	RIGHT_BRACKET_ERR,
	LEFT_BRACKET_ERR,
	FUNCTION_ERR,
	AMPERSAND_ERR,
	EXPRESSION_BRACKET_ERR,
	NEGATION_ERR,
	EXISTED_LABEL_ERR,
	NOT_EXISTED_LABEL_ERR,
	NOT_DECLARED_LABEL_ERR,
	LEFT_BRACE_ERR,
};
	

class ParsingException {
	char *lexeme;
	int string_number;
	int parse_error;
public:
	const char *get_error_lexeme() const { return lexeme; };
	const int get_error_string() const { return string_number; };
	const char *get_error_msg() const;
	ParsingException(struct lexemes *lex, int error);
	ParsingException(const ParsingException& other);
	~ParsingException();
};

ParsingException::ParsingException(struct lexemes *lex, int error) 
{
	lexeme=return_word(lex->word);
	string_number=lex->string_number;
	parse_error=error;
}

ParsingException::ParsingException(const ParsingException& other) 
{

	lexeme=return_word(other.lexeme);
	string_number=other.string_number;
	parse_error=other.parse_error;
}

ParsingException::~ParsingException() 
{
	delete[] lexeme;
}

const char *ParsingException::get_error_msg() const
{
	if(lexeme!=NULL) {
		char error_msg[50];
		switch (parse_error) {
			case STATEMENT_ERR:
				sprintf(error_msg,"expected operator");
				break;
				break;
			case SEMICOLON_ERR:
				sprintf(error_msg,"before the lexeme "
						"expected < ; >");
				break;
			case GAME_NAME_ERR:
				sprintf(error_msg,"expected game key word");
				break;
			case GAME_OPERATOR_ERR:
				sprintf(error_msg,"expected game operator");
				break;
			case KEY_WORD_ERR:
				sprintf(error_msg,"expected key word");
				break;
			case NULL_LIST_ERR:
				sprintf(error_msg,"after the lexeme the list "
			       			"is null");
				break;
			case PRINT_ERR:
				sprintf(error_msg,"expected print");
				break;
			case OPERAND_ERR:
				sprintf(error_msg,"expected operand");
				break;
			case SQUARE_BRACKET_ERR:
				sprintf(error_msg,"expected square bracket");
				break;
			case EQUAL_SIGN_ERR:
				sprintf(error_msg,"expected sign < = >");
				break;
			case IF_ERR:
				sprintf(error_msg,"expected key word < if >");
				break;
			case THEN_ERR:
				sprintf(error_msg,"expected key word < then >");
				break;
			case LABEL_ERR:
				sprintf(error_msg,"expected label");
				break;
			case GOTO_ERR:
				sprintf(error_msg,"expected key word < goto >");
				break;
			case VARIABLE_ERR:
				sprintf(error_msg,"expected variable");
				break;
			case DOUBLE_EQUAL_SIGN_ERR:
				sprintf(error_msg,"expected < == >");
				break;
			case DELIMITER_ERR:
				sprintf(error_msg,"expected < | >");
				break;
			case RIGHT_BRACKET_ERR:
				sprintf(error_msg,"expected < ( >");
				break;
			case LEFT_BRACKET_ERR:
				sprintf(error_msg,"expected < ) >");
				break;
			case FUNCTION_ERR:
				sprintf(error_msg,"expected function");
				break;
			case AMPERSAND_ERR:
				sprintf(error_msg,"expected < & >");
				break;
			case EXPRESSION_BRACKET_ERR:
				sprintf(error_msg,"expected < ) > after " 
							"the expression");
				break;
			case NEGATION_ERR:
				sprintf(error_msg,"expected < = > after " 
							"the negation");
				break;
			case EXISTED_LABEL_ERR:
				sprintf(error_msg,"this label "
						"already existed");
				break;
			case NOT_EXISTED_LABEL_ERR:
				sprintf(error_msg,"couldn`t jump "
						"using this label");
				break;
			case NOT_DECLARED_LABEL_ERR:
				sprintf(error_msg,"the label not declared");
				break;
			case LEFT_BRACE_ERR:
				sprintf(error_msg,"expected < { > after then");
				break;

		}
		return return_word(error_msg);
	} else {
		char list_over[50];
		sprintf(list_over,"Lexeme list was over\n");
		return return_word(list_over);
	}
}


class RPNElem;
class RPNLabel;
class RPNSysLabel;
class RPNInt;
class RPNVarAddr;


//=====================Classes that are in RPNElem========================//

class RPNEx {
	RPNElem *elem;
public:
	virtual void set_elem(RPNElem *element) {
		elem=element;
	}
	RPNElem *get_elem() {
		return elem;
	}
	virtual ~RPNEx() {}
};

class RPNExNotInt: public RPNEx {
public:
	RPNExNotInt(RPNElem *elem) {
		set_elem(elem);
	}
};

class RPNExNotVar: public RPNEx {
public:
	RPNExNotVar(RPNElem *elem) {
		set_elem(elem);
	}
};

class RPNExNotStringInt: public RPNEx {
public:
	RPNExNotStringInt(RPNElem *elem) {
		set_elem(elem);
	}
};

class RPNExNotLabel: public RPNEx {
public:
	RPNExNotLabel(RPNElem *elem) {
		set_elem(elem);
	}
};

class RPNExNotSysLabel: public RPNEx {
public:
	RPNExNotSysLabel(RPNElem *elem) {
		set_elem(elem);
	}
};


class RPNExNotVarTable: public RPNEx {
public:
	RPNExNotVarTable(RPNElem *elem) {
		set_elem(elem);
	}
};

class RPNExNotVarAddrVar: public RPNEx {
public:
	RPNExNotVarAddrVar(RPNElem *elem) {
		set_elem(elem);
	}
};

class RPNExDivideZero: public RPNEx {
public:
	RPNExDivideZero(RPNElem *elem) {
		set_elem(elem);
	}
};


struct RPNItem;


class RPNElem {
public:
	virtual ~RPNElem() {}
	virtual void Evaluate(RPNItem **stack, 
				RPNItem **curr_elem) const = 0;
	virtual void RPNPrint() const = 0;
	virtual RPNInt *popint(struct RPNItem **stack) const;

protected:
	static void push(RPNItem **stack, RPNElem *elem);
	static RPNElem *pop(RPNItem **stack);
};

struct RPNItem {
	int item_number;
	RPNElem *elem;
	RPNItem *next;
};

enum SerchInTables {
	FOUND,
	NOTFOUND,
	NOTDECLARED,
	DECLARED,
};

struct LabelItem {
	RPNItem **value;
	char *label_name;
	int declaration;
	struct LabelItem *next;
};

class LabelArrTable {
	struct LabelItem *label_table;
	struct LabelItem *label_last_table;
public:
	LabelArrTable() {
		label_table=NULL;
	}
	~LabelArrTable() {
		while(label_table!=NULL) {
			struct LabelItem *tmp=label_table;
			label_table=label_table->next;
			delete tmp;
		}
	}
	char *check_linked_labels() {
		struct LabelItem *tmp=label_table;
		while(tmp!=NULL) {
			if(tmp->declaration==NOTDECLARED) {
				return tmp->label_name;
			}
			tmp=tmp->next;
		}
		return NULL;
	}
	int check_label_in_table(char *name) {
		struct LabelItem *tmp=label_table;
		while(tmp!=NULL) {
			if(strcmp(tmp->label_name,name)==0) {
				tmp->declaration=DECLARED;
				return FOUND;
			}
			tmp=tmp->next;
		}
		return NOTFOUND;
	}
	RPNItem *get_label_addr(char *name) {
		struct LabelItem *tmp=label_table;
		while(tmp!=NULL) {
			if(strcmp(tmp->label_name,name)==0) {
				return *(tmp->value);
			}
			tmp=tmp->next;
		}
		return NULL;
	}
	void add_label_in_table(RPNItem **value, char *name) {
		if(label_table==NULL) {
			label_table=new LabelItem;
			label_last_table=label_table;
		} else {
			label_last_table->next=new LabelItem;
			label_last_table=label_last_table->next;
		}
		label_last_table->value=value;
		label_last_table->declaration=NOTDECLARED;
		label_last_table->label_name=new char [strlen(name)+1];
		int i=0;
		while(name[i]!='\0') {
			label_last_table->label_name[i]=name[i];
			i++;
		}
		label_last_table->label_name[i]='\0';
		label_last_table->next=NULL;
	}
	void output_label_table() {
		struct LabelItem *tmp=label_table;
		while(tmp!=NULL) {
			printf("Label name < %s > \n", tmp->label_name);
			tmp=tmp->next;
		}
	}

};

struct VarItem {
	int value;
	char *var_name;
	struct VarItem *next;
};

class VarArrTable {
	struct VarItem *var_table;
	struct VarItem *var_last_table;
public:
	VarArrTable() {
		var_table=NULL;
	}
	~VarArrTable() {
		while(var_table!=NULL) {
			struct VarItem *tmp=var_table;
			var_table=var_table->next;
			delete tmp;
		}
	}
	int check_var_in_table(char *name) {
		struct VarItem *tmp=var_table;
		while(tmp!=NULL) {
			if(strcmp(tmp->var_name,name)==0) {
				return FOUND;
			}
			tmp=tmp->next;
		}
		return NOTFOUND;
	}
	int get_value_from_table(char *name) {
		struct VarItem *tmp=var_table;
		while(tmp!=NULL) {
			if(strcmp(tmp->var_name,name)==0) {
				return tmp->value;
			}
			tmp=tmp->next;
		}
		return -1;
	}
	int set_value_in_table(char *name,int value) {
		struct VarItem *tmp=var_table;
		while(tmp!=NULL) {
			if(strcmp(tmp->var_name,name)==0) {
				tmp->value=value;
			}
			tmp=tmp->next;
		} 
		return ERROR;
	}
	void add_var_in_table(int value, char *name) {
		VarItem *tmp_table=var_table;
		while(tmp_table!=NULL) {
			if(strcmp(tmp_table->var_name,name)==0) {
				tmp_table->value=value;
				return;
			}
			tmp_table=tmp_table->next;
		}
		if(var_table==NULL) {
			var_table=new VarItem;
			var_last_table=var_table;
		} else {
			var_last_table->next=new VarItem;
			var_last_table=var_last_table->next;
		}
		var_last_table->value=value;
		var_last_table->var_name=new char [strlen(name)+1];
		int i=0;
		while(name[i]!='\0') {
			var_last_table->var_name[i]=name[i];
			i++;
		}
		var_last_table->var_name[i]='\0';
		var_last_table->next=NULL;
	}
	void output_var_table() {
		struct VarItem *tmp_table=var_table;
		while(tmp_table!=NULL) {
			printf("var_name=%s var_value=%d\n",
					tmp_table->var_name,tmp_table->value);
			tmp_table=tmp_table->next;
		}
	}
};

void RPNElem::push(RPNItem **stack,RPNElem *elem)
{
	RPNItem *tmp=new RPNItem;
	tmp->elem=elem;
	tmp->next=*stack;
	*stack=tmp;
}

RPNElem* RPNElem::pop(RPNItem **stack)
{
	RPNElem *tmp;
	tmp=(*stack)->elem;
	(*stack)=(*stack)->next;
	return tmp;
}

class RPNConst: public RPNElem {
public:
	virtual RPNElem *Clone() const = 0;
	virtual void Evaluate(struct RPNItem **stack,
			struct RPNItem **curr_elem) const {
		push(stack, Clone());
		(*curr_elem)=(*curr_elem)->next;
	}
};

class RPNInt: public RPNConst {
	int value;
public:
	RPNInt(int a) { 
		value=a; 
	}
	virtual ~RPNInt() {}
	virtual void RPNPrint() const {
		printf("%d RPNInt\n",value);
	}
	int Get() const {
		return value;
	}
	virtual RPNElem *Clone() const {
		return new RPNInt(value);
	}
	
};

RPNInt* RPNElem::popint(struct RPNItem **stack) const 
{
	RPNElem *operand1=pop(stack);
	RPNInt *i1=dynamic_cast<RPNInt*>(operand1);
	if(!i1) {
		throw new RPNExNotInt(operand1);
	}
	return i1;
}

class RPNFunction : public RPNElem {
public:
	virtual ~RPNFunction() {}
	virtual RPNElem *EvaluateFun(RPNItem **stack) const = 0;
	virtual void Evaluate(struct RPNItem **stack,
					struct RPNItem **curr_elem) const {
		RPNElem *res=EvaluateFun(stack);
		if(res!=NULL) {
			push(stack,res);
		}
		(*curr_elem)=(*curr_elem)->next;
	}
};

class RPNLabel: public RPNConst {
	RPNItem *value;
	char *label_name;
public:
	RPNLabel(RPNItem *a,char *name) {
		value=a;
		label_name=return_word(name);
	}
	virtual ~RPNLabel() {
		delete[] label_name;
	}
	virtual void RPNPrint() const {
		printf("%s point to item number < %d > RPNLabel\n",
				label_name, value->item_number);
	}
	RPNItem *Get() const {
		return value;
	}
	char *get_name() const {
		return label_name;
	}
	void set_addr(RPNItem *addr) {
		value=addr;
	}
	virtual RPNElem *Clone() const {
		return new RPNLabel(value,label_name);
	}
};

class RPNSysLabel: public RPNConst {
	RPNItem **value;
public:
	RPNSysLabel(RPNItem **a) {
		value=a;
	}
	virtual ~RPNSysLabel() {}
	virtual void RPNPrint() const {
		printf("point to item number < %d  > RPNSysLabel\n",
						(*value)->item_number);
	}
	RPNItem *Get() const {
		return *value;
	}
	virtual RPNElem *Clone() const {
		return new RPNSysLabel(value);
	}
};

class RPNOpGo: public RPNElem {
public:
	virtual void RPNPrint() const {
		printf("RPNOpGo\n");
	}
	virtual void Evaluate(struct RPNItem **stack,
			struct RPNItem **curr_elem) const {
		RPNElem *operand1=pop(stack);
		RPNLabel *label=dynamic_cast<RPNLabel*>(operand1);
		if(!label) {
			throw new RPNExNotLabel(operand1);
		}
		(*curr_elem)=label->Get();
		delete operand1;
	}
};


class RPNVarAddr: public RPNConst {
	VarArrTable *table;
	char *var_name;
	bool index;
public:
	RPNVarAddr(char *name,VarArrTable *t,bool ind) {
		table=t;
		var_name=return_word(name);
		index=ind;
		//table->output_var_table();
	}
	virtual RPNElem *Clone() const {
		if(index==false) {
			return new RPNInt(Get());
		} else {
			return new RPNVarAddr(var_name,table,index);
		} 
	}
	char *get_name() {
		return var_name;
	}
	void change_var_addr_name(char *name) {
		delete[] var_name;
		var_name=return_word(name);
	}
	void set_index(bool ind) {
		index=ind;
	}
	virtual void RPNPrint() const {
		printf("%s RPNVarAddr\n",var_name);
	}
	virtual ~RPNVarAddr() {
		delete[] var_name;
	}
	int Get() const {
		if(table->check_var_in_table(var_name)==NOTFOUND) {
			throw new RPNExNotVarTable(new 
					RPNVarAddr(var_name,table,index));
		}
		return table->get_value_from_table(var_name);
	}
	VarArrTable *get_table() {
		return table;
	}
};

class RPNFunVar : public RPNFunction {
	VarArrTable *table;
	char *var_name;
public:
	RPNFunVar(char *name,VarArrTable *t) {
		table=t;
		var_name=return_word(name);
	}
	virtual ~RPNFunVar() {
		delete[] var_name;
	}
	virtual void RPNPrint() const {
		printf("%s RPNFunVar\n",var_name);
	}
	void set_value(int value) {
		if(table->check_var_in_table(var_name)==NOTFOUND) {
			table->add_var_in_table(value,var_name);
		}
		table->set_value_in_table(var_name,value);
	}
	char *get_name() {
		return var_name;
	}
	VarArrTable *get_table() {
		return table;
	}
	RPNElem *Clone() {
		return new RPNFunVar(var_name,table);
	}
	virtual RPNElem *EvaluateFun(RPNItem **stack) const {
		return new RPNFunVar(var_name,table);
	}
};

class RPNIndex: public RPNElem {
public:
	virtual void RPNPrint() const {
		printf("RPNIndex\n");
	}
	virtual void Evaluate(struct RPNItem **stack,
			struct RPNItem **curr_elem) const {
		RPNInt *i1=popint(stack);
		RPNElem *operand1=pop(stack);
		RPNFunVar *var=dynamic_cast<RPNFunVar*>(operand1);
		if(var) {
			char *var_name=create_massif_var_name(var->get_name(),
								i1->Get());
			VarArrTable *table=var->get_table();
			push(stack,new RPNFunVar(var_name,table));
		} else {
			RPNVarAddr *var=dynamic_cast<RPNVarAddr*>(operand1);
			if(!var) {
				throw RPNExNotVarAddrVar(operand1);
			}
			char *var_name=create_massif_var_name(var->get_name(),
								i1->Get());
			var->change_var_addr_name(var_name);
			push(stack,new RPNInt(var->Get()));
		}
		delete operand1;
		delete i1;
		(*curr_elem)=(*curr_elem)->next;
	}
private:
	char *create_massif_var_name(char *string, int value) const {
		int res=value;
		int size=1;
		while(res!=0) {
			res=res/10;
			size++;
		}
		size=size+strlen(string);
		char *var_name=new char [size+1];
		sprintf(var_name,"%s%d",string,value);
		return var_name;
	}
};




class RPNOpGoFalse: public RPNElem {
public:
	virtual void RPNPrint() const {
		printf("RPNOpGoFalse\n");
	}
	virtual void Evaluate(struct RPNItem **stack,
			struct RPNItem **curr_elem) const {
		RPNElem *operand1=pop(stack);
		RPNSysLabel *label=dynamic_cast<RPNSysLabel*>(operand1);
		if(!label) {
			throw new RPNExNotSysLabel(operand1);
		}
		RPNInt *i1=popint(stack);
		if(i1->Get()>0) {
			(*curr_elem)=(*curr_elem)->next;
		} else {
			(*curr_elem)=label->Get();
		}
		delete operand1;
		delete i1;
	}
};

//==========================Classes that are in RPNConst===================//



class RPNString: public RPNConst {
	char *value;
public:
	RPNString(char *string) {
		value=return_word(string);
	}
	virtual ~RPNString() {
		delete[] value;
	}
	virtual void RPNPrint() const {
		printf("%s RPNString\n",value);
	}
	char *Get() const {
		return value;
	}
	virtual RPNElem *Clone() const {
		return new RPNString(value);
	}
};









//===========================Classes that are in RPNFucntion===============//




class RPNFunPlus : public RPNFunction {
public:
	RPNFunPlus() {}
	virtual ~RPNFunPlus() {}
	virtual RPNElem *EvaluateFun(RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int result=i1->Get()+i2->Get();
		delete i1;
		delete i2;
		return new RPNInt(result);
	}
	virtual void RPNPrint() const {
		printf("+ RPNFunPlus\n");
	}
};

class RPNFunMinus : public RPNFunction {
public:
	RPNFunMinus() {}
	virtual ~RPNFunMinus() {}
	virtual RPNElem *EvaluateFun(RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int result=i2->Get()-i1->Get();
		delete i1;
		delete i2;
		return new RPNInt(result);
	}
	virtual void RPNPrint() const {
		printf("- RPNFunMinus\n");
	}
};

class RPNFunDivide: public RPNFunction{
public:
	virtual ~RPNFunDivide() {}
	virtual RPNElem *EvaluateFun(RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int result=i2->Get()/i1->Get();
		delete i1;
		delete i2;
		return new RPNInt(result);
	}
	virtual void RPNPrint() const {
		printf("/ RPNFunDivide\n");
	}
};

class RPNFunMultiply: public RPNFunction {
public:
	virtual ~RPNFunMultiply() {}
	virtual RPNElem *EvaluateFun(RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int result=i2->Get()*i1->Get();
		delete i1;
		delete i2;
		return new RPNInt(result);
	}
	virtual void RPNPrint() const {
		printf("* RPNFunMultiply\n");
	}
};

class RPNFunOr: public RPNFunction {
public:
	virtual ~RPNFunOr() {}
	virtual void RPNPrint() const {
		printf("|| RPNFunOr\n");
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int res=i1->Get()||i2->Get();
		delete i1;
		delete i2;
		return new RPNInt(res);
	}		
};

class RPNFunAnd: public RPNFunction {
public:
	RPNFunAnd() {}
	virtual ~RPNFunAnd() {}
	virtual void RPNPrint() const {
		printf("&& RPNFunAnd\n");
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int res=i1->Get()&&i2->Get();
		delete i1;
		delete i2;
		return new RPNInt(res);
	}		
};

class RPNFunLess : public RPNFunction {
public:
	virtual void RPNPrint() const {
		printf("<\n RPNFunLess\n");
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int res;
		if(i2->Get()<i1->Get()) {
			res=1;
		} else {
			res=0;
		}
		delete i1;
		delete i2;
		return new RPNInt(res);
	}		
};

class RPNFunMore : public RPNFunction {
public:
	virtual void RPNPrint() const {
		printf("> RPNFunMore\n");
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int res;
		if(i2->Get()>i1->Get()) {
			res=1;
		} else {
			res=0;
		}
		delete i1;
		delete i2;
		return new RPNInt(res);
	}		
};

class RPNFunEqual : public RPNFunction {
public:
	virtual void RPNPrint() const {
		printf("== RPNFunEqual\n");
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int res;
		if(i2->Get()==i1->Get()) {
			res=1;
		} else {
			res=0;
		}
		delete i1;
		delete i2;
		return new RPNInt(res);
	}		
};

class RPNFunNot : public RPNFunction {
public:
	virtual void RPNPrint() const {
		printf("!= RPNFunNot\n");
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		int res;
		if(i2->Get()!=i1->Get()) {
			res=1;
		} else {
			res=0;
		}
		delete i1;
		delete i2;
		return new RPNInt(res);
	}		
};

class RPNFunAssignment : public RPNFunction {
public:
	virtual void RPNPrint() const {
		printf("= RPNFunAssignment\n");
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNElem *operand2=pop(stack);
		RPNFunVar *var=dynamic_cast<RPNFunVar*>(operand2);
		if(!var) {
			throw new RPNExNotVar(operand2);
		}
		var->set_value(i1->Get());
		delete i1;
		delete operand2;
		return NULL;

	}
};

class RPNFunPrint : public RPNFunction {
public:
	virtual void RPNPrint() const {
		printf("print RPNFunPrint\n");
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNElem *operand1=pop(stack);
		RPNInt *i1=dynamic_cast<RPNInt*>(operand1);
		if(!i1) {
			RPNString *i1=dynamic_cast<RPNString*>(operand1);
			if(!i1) { 
				throw new RPNExNotStringInt(operand1);
			} else {
				char *string=return_word(i1->Get());
				int size=strlen(string);
				if(string[size-2]=='\\'&&
				string[size-1]=='n') {
					string[size-2]='\0';
					printf("%s\n",string);
				} else {
					printf("%s",string);
				}
				delete[] string;
			}
		} else {
			printf("%d",i1->Get());
		}
		delete operand1;
		return NULL;
	}
};

class RPNFunGameNameZero : public RPNFunction {
	char *action;
	Server *serv;
public:
	RPNFunGameNameZero(char *string, Server *server) {
		action=return_word(string);
		serv=server;
	}
	virtual ~RPNFunGameNameZero() {
		delete[] action;
	}
	virtual void RPNPrint() const {
		printf("%s RPNFunGameNameZero\n",action);
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		serv->make_game_name_zero(action);
		return NULL;
	}
};

class RPNFunGameNameOne : public RPNFunction {
	char *action;
	Server *serv;
public:
	RPNFunGameNameOne(char *string, Server *server) {
		action=return_word(string);
		serv=server;
	}
	virtual ~RPNFunGameNameOne() {
		delete[] action;
	}
	virtual void RPNPrint() const {
		printf("%s RPNFunGameNameOne\n",action);
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		serv->make_game_name_one(action,i1->Get());
		delete i1;
		return NULL;
	}		
};

class RPNFunGameNameTwo : public RPNFunction {
	char *action;
	Server *serv;
public:
	RPNFunGameNameTwo(char *string,Server *server) {
		action=return_word(string);
		serv=server;
	}
	virtual ~RPNFunGameNameTwo() {
		delete[] action;
	}
	virtual void RPNPrint() const {
		printf("%s RPNFunGameNameTwo\n",action);
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const {
		RPNInt *i1=popint(stack);
		RPNInt *i2=popint(stack);
		serv->make_game_name_two(action,i2->Get(),i1->Get());
		delete i1;
		delete i2;
		return NULL;
	}		

};


class RPNFunServer : public RPNFunction {
	char *function_name;
	Server *serv;
public:
	RPNFunServer(char *string, Server *server) {
		function_name=return_word(string);
		serv=server;
	}
	virtual ~RPNFunServer() {
		delete[] function_name;
	}
	virtual void RPNPrint() const {
		printf("%s RPNFunServer\n",function_name);
	}
	virtual RPNElem *EvaluateFun(struct RPNItem **stack) const;

};


RPNElem* RPNFunServer::EvaluateFun(struct RPNItem **stack) const 
{
	if(find_table_string(function_name,func_without_param)==0) {
		int res=serv->check_func_without_param(function_name);
		return new RPNInt(res);
	} else 
	if(find_table_string(function_name,func_with_param)==0) {
		RPNInt *i1=popint(stack);
		int res=serv->check_func_with_param(function_name,
								i1->Get());
		delete i1;
		return new RPNInt(res);
	} else {
		return NULL;
	}
}

//===========================================================================//


class Parsing {
	struct lexemes *lex_list;

	struct RPNItem *rpn_list;
	struct RPNItem *rpn_last_list;
	int item_number;
	VarArrTable *VT;
	LabelArrTable LT; 
	Server *serv;

	static const char *game_actions[];
	static const char *game_name_zero[];
	static const char *game_name_one[];
	static const char *game_name_two[];
public:
	Parsing(VarArrTable *VarTable, Server *server) {
		VT=VarTable;
		serv=server;
		item_number=0;
		rpn_list=new RPNItem;
		rpn_last_list=rpn_list;
	}
	RPNItem *parse_lexeme_list(struct lexemes *lexeme_list);
private:
	void next_link();
	void function_operator();
	void operand_operator();
	void expression_two_operator();
	void expression_one_operator();
	void expression_operator();
	void var_operator();
	void assignment_operator();
	void print_list_operator();
	void print_operator();
	void game_operator();
	void conditional_operator();
	void cycle_operator();
	void goto_operator();
	void statements();
	void block_statement();
	void statement();
	void grammar_purpose();


	void link_labels(); 
	void linking_error(char *name, int error);
	void set_index_in_var_addr();

	void create_rpn_list(RPNElem *elem);

};

const char* Parsing::game_actions[]={"endturn","prod","build",
						"buy","sell",NULL};
const char* Parsing::game_name_zero[]={"endturn",NULL};
const char* Parsing::game_name_one[]={"prod","build",NULL};
const char* Parsing::game_name_two[]={"buy","sell",NULL};

void Parsing::create_rpn_list(RPNElem *elem)
{
	item_number++;
	rpn_last_list->next=new RPNItem;
	rpn_last_list=rpn_last_list->next;
	rpn_last_list->elem=elem;
	rpn_last_list->item_number=item_number;
	rpn_last_list->next=NULL;
}

void Parsing::next_link()
{
	if(lex_list==NULL) {
		return;
	} else {
		if(lex_list->next==NULL) {
			if(strcmp(lex_list->word,";")!=0&&
				strcmp(lex_list->word,"}")!=0) {
				throw ParsingException(lex_list,NULL_LIST_ERR);
			}
		}
		lex_list=lex_list->next;
	}
}

void Parsing::function_operator()
{	if(lex_list->type==FUNCTION) {
		struct lexemes *tmp=lex_list;
		next_link();
		if(strcmp(lex_list->word,"(")!=0) {
			throw ParsingException(lex_list,RIGHT_BRACKET_ERR);
		}
		next_link();
		if(lex_list->type==NUMBER||lex_list->type==FUNCTION||
		lex_list->type==VARIABLE) {
			expression_operator();
		}
		if(strcmp(lex_list->word,")")!=0) {
			throw ParsingException(lex_list,LEFT_BRACKET_ERR);
		}
		next_link();
		create_rpn_list(new RPNFunServer(tmp->word,serv));
	} else {
		throw ParsingException(lex_list,FUNCTION_ERR);
	}
}

void Parsing::operand_operator()
{
	if(lex_list->type==NUMBER) {
		create_rpn_list(new RPNInt(atoi(lex_list->word)));
		next_link();
	} else
	if(lex_list->type==VARIABLE) {
		create_rpn_list(new RPNVarAddr(lex_list->word,VT,false));
		var_operator();
	} else
	if(lex_list->type==FUNCTION) {
		function_operator();
	} else 
	if(strcmp(lex_list->word,"(")==0) {
		next_link();
		expression_operator();
		if(strcmp(lex_list->word,")")!=0) {
			throw ParsingException(lex_list,EXPRESSION_BRACKET_ERR);
		}
		next_link();
	} else {
		throw ParsingException(lex_list,OPERAND_ERR);
	}
}


void Parsing::expression_two_operator()
{
	operand_operator();
	while(strcmp(lex_list->word,"*")==0||
	strcmp(lex_list->word,"/")==0||
	strcmp(lex_list->word,"&")==0) {
		if(strcmp(lex_list->word,"*")==0) {
			next_link();
			operand_operator();
			create_rpn_list(new RPNFunMultiply());
		} else
		if(strcmp(lex_list->word,"/")==0) {
			next_link();
			operand_operator();
			create_rpn_list(new RPNFunDivide());
		} else 
		if(strcmp(lex_list->word,"&")==0) {
			next_link();
			if(strcmp(lex_list->word,"&")!=0) {
				throw ParsingException(lex_list,AMPERSAND_ERR);
			}
			next_link();
			operand_operator();
			create_rpn_list(new RPNFunAnd());
		}
	}
}

void Parsing::expression_one_operator()
{
	expression_two_operator();
	while(strcmp(lex_list->word,"+")==0||
	strcmp(lex_list->word,"-")==0||
	strcmp(lex_list->word,"|")==0) {
		if(strcmp(lex_list->word,"+")==0) {
			next_link();
			expression_two_operator();
			create_rpn_list(new RPNFunPlus());
		} else 
		if(strcmp(lex_list->word,"-")==0) {
			next_link();
			expression_two_operator();
			create_rpn_list(new RPNFunMinus());
		} else
		if(strcmp(lex_list->word,"|")==0) {
			next_link();
			if(strcmp(lex_list->word,"|")!=0) {
				throw ParsingException(lex_list,DELIMITER_ERR);
			}
			next_link();
			expression_two_operator();
			create_rpn_list(new RPNFunOr());
		}
	}
}

void Parsing::expression_operator()
{
	expression_one_operator();
	if(strcmp(lex_list->word,"=")==0) {
		next_link();
		if(strcmp(lex_list->word,"=")!=0) {
			throw ParsingException(lex_list,DOUBLE_EQUAL_SIGN_ERR);	
		}
		next_link();
		expression_one_operator();
		create_rpn_list(new RPNFunEqual());
	} else
	if(strcmp(lex_list->word,"<")==0) {
		next_link();
		expression_one_operator();
		create_rpn_list(new RPNFunLess());
	} else
	if(strcmp(lex_list->word,">")==0) {
		next_link();
		expression_one_operator();
		create_rpn_list(new RPNFunMore());
	} else
	if(strcmp(lex_list->word,"!")==0) {
		next_link();
		if(strcmp(lex_list->word,"=")!=0) {
			throw ParsingException(lex_list,NEGATION_ERR);
		}
		next_link();
		expression_one_operator();
		create_rpn_list(new RPNFunNot());
	}
}

void Parsing::var_operator()
{
	if(lex_list->type==VARIABLE) {
		next_link();
		if(strcmp(lex_list->word,"[")==0) {
			next_link();
			expression_operator();
			create_rpn_list(new RPNIndex);
			if(strcmp(lex_list->word,"]")!=0) {
				throw ParsingException(lex_list,
						SQUARE_BRACKET_ERR);
			}	
			next_link();
		}
	} else {
		throw ParsingException(lex_list,VARIABLE_ERR);
	}
}

void Parsing::assignment_operator()
{
	create_rpn_list(new RPNFunVar(lex_list->word,VT));
	var_operator();
	if(strcmp(lex_list->word,"=")==0) {
		next_link();
		expression_operator();
		create_rpn_list(new RPNFunAssignment());
	} else {
		throw ParsingException(lex_list,EQUAL_SIGN_ERR);
	}
}

void Parsing::print_list_operator() 
{
	if(lex_list->type==STRINGCONST) {
		create_rpn_list(new RPNString(lex_list->word));
		next_link();
	} else {
		expression_operator();
	}
	create_rpn_list(new RPNFunPrint());
	if(strcmp(lex_list->word,",")==0) {
		next_link();
		print_list_operator();
	}
}

void Parsing::print_operator()
{
	if(strcmp(lex_list->word,"print")==0) {
		next_link();
		print_list_operator();
	} else { 
		throw ParsingException(lex_list,PRINT_ERR);
	}
}

void Parsing::game_operator()
{
	if(lex_list->type==KEYWORD) {
		if(find_table_string(lex_list->word,game_name_zero)==0) {
			create_rpn_list(new 
					RPNFunGameNameZero(lex_list->word,
								serv));
			next_link();
		} else
		if(find_table_string(lex_list->word,game_name_one)==0) {
			struct lexemes *tmp=lex_list;
			next_link();
			operand_operator();
			create_rpn_list(new 
					RPNFunGameNameOne(tmp->word,serv));
		} else
		if(find_table_string(lex_list->word,game_name_two)==0) {
			struct lexemes *tmp=lex_list;
			next_link();
			operand_operator();
			operand_operator();
			create_rpn_list(new 
					RPNFunGameNameTwo(tmp->word,serv));
		} else {
			throw ParsingException(lex_list,GAME_NAME_ERR);
		}
	} else {
		throw ParsingException(lex_list,KEY_WORD_ERR);
	}
}

void Parsing::conditional_operator()
{
	if(strcmp(lex_list->word,"if")==0) {
		next_link();
		expression_operator();
		create_rpn_list(NULL);
		RPNItem *tmp=rpn_last_list;
		create_rpn_list(new RPNOpGoFalse);
		if(strcmp(lex_list->word,"then")!=0) {
			throw ParsingException(lex_list,THEN_ERR);
		}
		next_link();
		if(strcmp(lex_list->word,"{")==0) {
			next_link();
			block_statement();
			next_link();
		} else {
			throw ParsingException(lex_list,LEFT_BRACE_ERR);
		}
		tmp->elem=new RPNSysLabel(&rpn_last_list->next);

	} else {
		throw ParsingException(lex_list,IF_ERR);
	}
}

void Parsing::cycle_operator()
{
	if(lex_list->type==LABEL) {
		if(LT.check_label_in_table(lex_list->word)==NOTFOUND) {
			LT.add_label_in_table(&rpn_last_list->next,
							lex_list->word);
		} else {
			throw ParsingException(lex_list,EXISTED_LABEL_ERR);
		}
	}
	next_link();
}

void Parsing::goto_operator()
{
	if(strcmp(lex_list->word,"goto")!=0) {
		throw ParsingException(lex_list,GOTO_ERR);
	}
	next_link();
	if(lex_list->type==LABEL) {
		create_rpn_list(new RPNLabel(NULL,
				lex_list->word));
		next_link();
	} else {
		throw ParsingException(lex_list,LABEL_ERR);
	}
	create_rpn_list(new RPNOpGo);
}

void Parsing::block_statement() 
{
	if(strcmp(lex_list->word,"}")!=0) {
		statement();
		block_statement();
	}
}


void Parsing::statement()
{
	if(lex_list->type==LABEL) {
		cycle_operator();
	} else 
	if(lex_list->type==KEYWORD&&strcmp(lex_list->word,"if")==0) {
		conditional_operator();
	} else {
		if(lex_list->type==VARIABLE) {
			assignment_operator();
		} else
		if(lex_list->type==KEYWORD) {
			if(strcmp(lex_list->word,"print")==0) {
				print_operator();
			} else 
			if(find_table_string(lex_list->word,
						game_actions)==0) {
				game_operator();
			} else
			if(strcmp(lex_list->word,"goto")==0) {
				goto_operator();
			} 
		} else {
			throw ParsingException(lex_list,STATEMENT_ERR);
		}
		if(strcmp(lex_list->word,";")!=0) {
			throw ParsingException(lex_list,SEMICOLON_ERR);
		}
		next_link();
	}
}

void Parsing::statements() {
	if(lex_list!=NULL) {
		statement();
		statements();
	}
}

void Parsing::grammar_purpose() {
	if(lex_list!=NULL) {
		if(lex_list->type==LABEL) {
			if(LT.check_label_in_table(lex_list->word)==NOTFOUND) {
				LT.add_label_in_table(&rpn_last_list->next,
							lex_list->word);
			} else {
				throw ParsingException(lex_list,
						EXISTED_LABEL_ERR);
			}
			next_link();
		}
		statement();
		statements();
	} 
}


void Parsing::linking_error(char *name, int error) 
{
	struct lexemes *lex_tmp=lex_list;
	while(lex_tmp!=NULL) {
		if(lex_tmp->type==LABEL&&
		strcmp(lex_tmp->word,name)==0) {
			break;
		}
		lex_tmp=lex_tmp->next;
	}
	delete[] name;
	throw ParsingException(lex_tmp,error);
}

void Parsing::link_labels() 
{
	struct RPNItem *rpn_tmp=rpn_list;
	while(rpn_tmp!=NULL) {
		RPNLabel *label=dynamic_cast<RPNLabel*>(rpn_tmp->elem);
		if(label) {
			char *name=label->get_name();
			if(LT.check_label_in_table(name)==NOTFOUND) {
				linking_error(name,NOT_EXISTED_LABEL_ERR);
			} else {
				label->set_addr(LT.get_label_addr(name));
			}
		}
		rpn_tmp=rpn_tmp->next;
	}
	char *name;
	if((name=LT.check_linked_labels())!=NULL)	{
		linking_error(name,NOT_DECLARED_LABEL_ERR);
	}
}

void Parsing::set_index_in_var_addr() 
{
	struct RPNItem *rpn_tmp=rpn_list;
	while(rpn_tmp!=NULL) {
		RPNVarAddr *var_addr=dynamic_cast<RPNVarAddr*>(rpn_tmp->elem);
		if(var_addr&&(rpn_tmp->next!=NULL)&&
			(rpn_tmp->next->next!=NULL)) 
		{
			RPNIndex *index=
				dynamic_cast<RPNIndex*>
				(rpn_tmp->next->next->elem);
			if(index) {
				var_addr->set_index(true);
			}
		}
		rpn_tmp=rpn_tmp->next;
	}
}

RPNItem*  Parsing::parse_lexeme_list(struct lexemes *lexeme_list)
{
	lex_list=lexeme_list;
	if(lex_list!=NULL) {
		grammar_purpose();
	} else {
		printf("Lexeme list is empty\n");
	}
	lex_list=lexeme_list;
	rpn_list=rpn_list->next;
	link_labels();
	set_index_in_var_addr();
	return rpn_list;
}

void find_got_elem(RPNEx *EX) 
{
	if(dynamic_cast<RPNOpGo*>(EX->get_elem())) {
		printf("but got RPNOpGo\n");
	} else 
	if(dynamic_cast<RPNOpGoFalse*>(EX->get_elem())) {
		printf("but got RPNOpGoFalse\n");
	} else 
	if(dynamic_cast<RPNInt*>(EX->get_elem())) {
		printf("but got RPNInt\n");
	} else 
	if(dynamic_cast<RPNString*>(EX->get_elem())) {
		printf("but got RPNString\n");
	} else 
	if(dynamic_cast<RPNLabel*>(EX->get_elem())) {
		printf("but got RPNLabel\n");
	} else 
	if(dynamic_cast<RPNSysLabel*>(EX->get_elem())) {
		printf("but got RPNSysLabel\n");
	} else 
	if(dynamic_cast<RPNVarAddr*>(EX->get_elem())) {
		printf("but got RPNVarAddr\n");
	} else 
	if(dynamic_cast<RPNFunVar*>(EX->get_elem())) {
		printf("but got RPNFunVar\n");
	} else 
	if(dynamic_cast<RPNFunPlus*>(EX->get_elem())) {
		printf("but got RPNFunPlus\n");
	} else 
	if(dynamic_cast<RPNFunMinus*>(EX->get_elem())) {
		printf("but got RPNFunMinus\n");
	} else 
	if(dynamic_cast<RPNFunDivide*>(EX->get_elem())) {
		printf("but got RPNFunDivide\n");
	} else 
	if(dynamic_cast<RPNFunMultiply*>(EX->get_elem())) {
		printf("but got RPNFunMultiply\n");
	} else 
	if(dynamic_cast<RPNFunOr*>(EX->get_elem())) {
		printf("but got RPNFunOr\n");
	} else 
	if(dynamic_cast<RPNFunAnd*>(EX->get_elem())) {
		printf("but got RPNFunAnd\n");
	} else 
	if(dynamic_cast<RPNFunLess*>(EX->get_elem())) {
		printf("but got RPNFunLess\n");
	} else 
	if(dynamic_cast<RPNFunMore*>(EX->get_elem())) {
		printf("but got RPNFunMore\n");
	} else 
	if(dynamic_cast<RPNFunEqual*>(EX->get_elem())) {
		printf("but got RPNFunEqual\n");
	} else 
	if(dynamic_cast<RPNFunNot*>(EX->get_elem())) {
		printf("but got RPNFunNot\n");
	} else 
	if(dynamic_cast<RPNFunAssignment*>(EX->get_elem())) {
		printf("but got RPNFunAssignment\n");
	} else 
	if(dynamic_cast<RPNFunPrint*>(EX->get_elem())) {
		printf("but got RPNFunPrint\n");
	} else 
	if(dynamic_cast<RPNFunGameNameZero*>(EX->get_elem())) {
		printf("but got RPNFunGameNameZero\n");
	} else 
	if(dynamic_cast<RPNFunGameNameOne*>(EX->get_elem())) {
		printf("but got RPNFunGameNameOne\n");
	} else 
	if(dynamic_cast<RPNFunGameNameTwo*>(EX->get_elem())) {
		printf("but got RPNFunGameNameTwo\n");
	} else 
	if(dynamic_cast<RPNFunServer*>(EX->get_elem())) {
		printf("but got RPNFunServer\n");
	} else  
	if(dynamic_cast<RPNIndex*>(EX->get_elem())) {
		printf("but got RPNIndex\n");
	} 
}	
	
void find_error_in_rpn(RPNEx *EX)
{
	if(dynamic_cast<RPNExNotInt*>(EX)) {
		printf("In RPN interpretation: " 
			"expected integer value ");
		find_got_elem(EX);
	} else
	if(dynamic_cast<RPNExNotStringInt*>(EX)) {
		printf("In RPN interpretation: " 
			"expected integer value or string constant ");
		find_got_elem(EX);
	} else 
	if(dynamic_cast<RPNExNotLabel*>(EX)) {
		printf("In RPN interpretation: " 
			"expected integer value ");
		find_got_elem(EX);
	} else 
	if(dynamic_cast<RPNExNotSysLabel*>(EX)) {
		printf("In RPN interpretation: " 
			"expected integer value ");
		find_got_elem(EX);
	} else 
	if(dynamic_cast<RPNExNotVar*>(EX)) {
		printf("In rpn interpretation: " 
			"expected variable ");
		find_got_elem(EX);
	} else 
	if(dynamic_cast<RPNExNotVarTable*>(EX)) {
		RPNVarAddr *i1=dynamic_cast<RPNVarAddr*>(EX->get_elem());
		printf("In rpn interpretation: " 
			"variable < %s > is not initialized\n",
						i1->get_name());
	} else 
	if(dynamic_cast<RPNExNotVarAddrVar*>(EX)) {
		printf("In rpn interpretation: " 
			"expected variable or variable address ");
		find_got_elem(EX);
	} else
	if(dynamic_cast<RPNExDivideZero*>(EX)) {
		printf("In rpn interpretation: " 
			"the second operand in the divide is 0 ");
	} 
}




//=========================================================================//




















void start_interpretation(struct RPNItem *rpn_list, Server *serv)
{
	struct RPNItem *stack=NULL, *curr_elem=rpn_list;
	while(serv->check_end_game()==false) {
		serv->work_with_socket();
		if(serv->check_all_info()==INFO) {
			while(curr_elem!=NULL) {
				//curr_elem->elem->RPNPrint();
				struct RPNItem *rpn_tmp=curr_elem;
				curr_elem->elem->Evaluate(&stack,&curr_elem);
				if(dynamic_cast<RPNFunGameNameZero*>(
							rpn_tmp->elem)) {
					break;
				}
			}
			serv->start_of_turn();
		}
	}
}

int main(int argc, char **argv)
{
	check_parameters(argc,argv);
	Server serv(argc,argv);
	if(serv.work_with_init_data(argv)==ERROR) {
		exit(1);
	}
	struct lexemes *lex_list;
	VarArrTable VarTable;
	Parsing PA(&VarTable,&serv);
	RPNItem *rpn_list;
	FILE* fd=open_file(argv);
	if((lex_list=lexical_analysis(fd))==NULL) {
		printf("The list is empty\n");
		exit(0);
	}
	fclose(fd);
	try {
		rpn_list=PA.parse_lexeme_list(lex_list);
		printf("The list successfully parsed\n");
		delete_lexeme_list(&lex_list);
		/*while(rpn_list!=NULL) {
			printf("%d) ",rpn_list->item_number);
			rpn_list->elem->RPNPrint();
			rpn_list=rpn_list->next;
		}*/
		start_interpretation(rpn_list,&serv);
	}
	catch(const ParsingException &PE) {
		fprintf(stderr,"Error has occured in lexeme < %s >, "
				"string number < %d >, %s\n",
				PE.get_error_lexeme(), PE.get_error_string(),
				PE.get_error_msg());
		exit(1);
	}
	catch(RPNEx *EX) {
		find_error_in_rpn(EX);
	}
	return 0;
}

