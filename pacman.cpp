#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <cmath>

using namespace std;

const float FPS = 6;
const int SCREEN_W = 500;
const int SCREEN_H = 550;

enum MYKEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

//matriz definindo mapa do jogo: 1 representa paredes, 0 representa corredor
char MAPA[26][26] = {
	"1111111111111111111111111",
	"1000000001111111000000001",
	"1011111101111111011111101",
	"1011111100000000011111101",
	"1000000001111111000000001",
	"1110111101111111011110111",
	"1000111100001000011110001",
	"1010111111101011111110101",
	"1010000111001001110000101",
	"1011110111011101110111101",
	"1011110100000000010111101",
	"1000110001110111000110001",
	"1110111101110111011110111",
	"1000000001110111000000001",
	"1011111101110111011111101",
	"1011100000000000000011101",
	"1000001111110111111000001",
	"1111101110000000111011111",
	"1111101110111110111011111",
	"1000000000000000000000001",
	"1011101111110111111011101",
	"1000001111110111111000001",
	"1011100000010100000011101",
	"1000001111000001111000001",
	"1111111111111111111111111",
};

ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_FONT* fonte = NULL;
ALLEGRO_EVENT_QUEUE* event_queue = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_BITMAP* wall = NULL;
ALLEGRO_BITMAP* pill = NULL;
ALLEGRO_BITMAP* ghost1 = NULL;
ALLEGRO_BITMAP* ghost2 = NULL;
ALLEGRO_BITMAP* ghost3 = NULL;
ALLEGRO_BITMAP* ghost4 = NULL;
ALLEGRO_BITMAP* pacman = NULL;
int i = 15, j = 12; //posicao inicial do Pacman na matriz
int q = 20; //tamanho de cada celula no mapa
int posy = i * q;
int posx = j * q;

bool key[4] = {false, false, false, false};
bool redraw = true;
bool sair = false;
int pontos = 0;

// Variável que guarda de a jogada foi feita ou não
bool jogada = false;

// Variáveis dos tipos de jogadas de cada fantasma
int v1 = 0, v2 =0, v3 = 0, v4 = 0;

//Posições iniciais dos fantasmas
// Fantasma 1
int g1i = 1;
int g1j = 1;

// Fantasma 2
int g2i = 1;
int g2j = 23;

// Fantasma 3
int g3i = 23;
int g3j = 1;

// Fantasma 4
int g4i = 23;
int g4j = 23;

//Marcador de posição dos fantasmas
//Fantasma 1
int g1posy = g1i * q;
int g1posx = g1j * q;

//Fantasma 2
int g2posy = g2i * q;
int g2posx = g2j * q;

//Fantasma 3
int g3posy = g3i * q;
int g3posx = g3j * q;

//Fantasma 1
int g4posy = g4i * q;
int g4posx = g4j * q;
// -------------------------------------------------------------------------------------------
void carregarMapa() {
	for(int i = 0; i < 26; i++) {
		for(int j = 0; j < 26; j++) {
			if(MAPA[i][j] == '1') {
				al_draw_bitmap(wall, j * q, i * q, 0);
			} else if(MAPA[i][j] == '0') {
				if(posx == j * q && posy == i * q) {
					MAPA[i][j] = 'a';
					if(posx != 12 * 20 || posy != 15 * 20) {
						pontos += 10;
					}
					
				} else {
					al_draw_bitmap(pill, j * q, i * q, 0);
				}
			}
			/* if((i == 1 && j == 1) || (i == 1 && j == 23) || (i == 23 && j == 1) || (i == 23 && j == 23)) {
				al_draw_bitmap(ghost, j * q, i * q, 0);
			} */
			if(i == g1i && j == g1j){
    			al_draw_bitmap(ghost1, g1posx, g1posy, 0);
			}
			if(i == g2i && j == g2j){
    			al_draw_bitmap(ghost2, g2posx, g2posy, 0);
			}
			if(i == g3i && j == g3j){
    			al_draw_bitmap(ghost3, g3posx, g3posy, 0);
			}
			// Fantasma 4 deve ser inteligente
			if(i == 23 && j == 23){
    			al_draw_bitmap(ghost4, j * q, i * q, 0);
			}
		}
	}
	al_draw_textf(fonte, al_map_rgb(255, 255, 255), 5, 5, ALLEGRO_ALIGN_LEFT, "PONTOS: %d", pontos);
}

int inicializa() {
	if(!al_init()) {
		cout << "Falha ao carregar Allegro" << endl;
		return 0;
	}

	if(!al_install_keyboard()) {
		cout << "Falha ao inicializar o teclado" << endl;
		return 0;
	}

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		cout << "Falha ao inicializar o temporizador" << endl;
		return 0;
	}

	if(!al_init_image_addon()) {
		cout << "Falha ao iniciar al_init_image_addon!" << endl;
		return 0;
	}

	if (!al_init_font_addon()) {
		cout << "Falha ao iniciar al_init_font_addon!" << endl;
		return 0;
	}

	if (!al_init_ttf_addon()) {
		cout << "Falha ao inicializar add-on allegro_ttf";
		return 0;
	}

	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		cout << "Falha ao inicializar a tela" << endl;
		al_destroy_timer(timer);
		return 0;
	}

	fonte = al_load_font("arial.ttf", 12, 0);
	if (!fonte) {
		cout << "Falha ao carregar a fonte!" << endl;
		al_destroy_display(display);
		return 0;
	}

	// mapa = al_load_bitmap("map.bmp");
	// if(!mapa)
	// {
	//     cout << "Falha ao carregar o mapa!" << endl;
	//     al_destroy_display(display);
	//     return 0;
	// }
	// al_draw_bitmap(mapa,0,0,0);

	pacman = al_load_bitmap("pacmanright.tga");
	if(!pacman) {
		cout << "Falha ao carregar o pacman!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(pacman, posx, posy, 0);

	event_queue = al_create_event_queue();
	if(!event_queue) {
		cout << "Falha ao criar a fila de eventos" << endl;
		al_destroy_display(display);
		al_destroy_timer(timer);
		return 0;
	}

	//cria as paredes do mapa e as pilulas e os fantasmas

	wall = al_create_bitmap(q, q);
	pill = al_load_bitmap("pill.tga");
	ghost1 = al_load_bitmap("ghost.tga");
	ghost2 = al_load_bitmap("ghost.tga");
	ghost3 = al_load_bitmap("ghost.tga");
	ghost4 = al_load_bitmap("ghost.tga");
	if(!wall || !pill) {
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	al_set_target_bitmap(wall);
	al_clear_to_color(al_map_rgb(0, 0, 255));
	al_set_target_bitmap(al_get_backbuffer(display));

	//****************************************************************//

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_flip_display();
	al_start_timer(timer);

	return 1;
}
//--------------------------------------------------------------------------------------------
// Essa função verifica se o movimento que o fantasma aleatório deseja fazer é possível
bool possivel(int jog, int i, int j){
	switch (jog){
	case 0:
		if(MAPA[i - 1][j] != '1'){
			return true;
		}else{
			return false;
		}
		break;
	case 1:
		if(MAPA[i + 1][j] != '1'){
			return true;
		}else{
			return false;
		}
		break;
	case 2:
		if(MAPA[i][j - 1] != '1'){ 
			return true;
		}else{
			return false;
		}
		break;
	case 3:
		if(MAPA[i][j + 1] != '1'){
			return true;
		}else{
			return false;
		}
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------------------------
// Função que calcula a distância entre o pacman e o fantasma

int calculaDistancia(){
	int distancia = 0;

	distancia = sqrt(pow((posx - g4posx),2)+ pow((posy - g4posy),2));

	return distancia;
}

// Funções de movimentação dos fantasmas aleatórios ------------------------------------------
void movimentaG1(){
	bool jogadaRealizada = false;
	bool jogadaPossivel = false;

	while(!jogadaRealizada){
		inicio:
		jogadaPossivel = possivel(v1, g1i, g1j);
		if(jogadaPossivel){
			goto joga;
		}else{
			v1 = rand() % 4;
			goto inicio;
		}
		// ---------------------------------------------------------------------------------------------
		joga:
		switch(v1){
		case 0:
			g1i--;
			g1posy = g1i * q;
			ghost1 = al_load_bitmap("ghost.tga");
			//verificaMorto();
			v1 = 0;
			jogadaRealizada = true;
			break;
		case 1:
			g1i++;
			g1posy = g1i * q;
			ghost1 = al_load_bitmap("ghost.tga");
			//verificaMorto();
			v1=1;
			jogadaRealizada = true;
			break;
		case 2:
			g1j--;
			g1posx = g1j * q;
			ghost1 = al_load_bitmap("ghost.tga"); 
			//verificaMorto();
			v1=2;
			jogadaRealizada = true;
			break;
		case 3:
			g1j++;
			g1posx = g1j * q;
			ghost1 = al_load_bitmap("ghost.tga"); 
			//verificaMorto();
			v1=3;
			jogadaRealizada = true;
			break;		
		}
	}
}
void movimentaG2(){
	bool jogadaRealizada = false;
	bool jogadaPossivel = false;

	while(!jogadaRealizada){
		inicio:
		jogadaPossivel = possivel(v2, g2i, g2j);
		if(jogadaPossivel){
			goto joga;
		}else{
			v2 = rand() % 4;
			goto inicio;
		}
		// ---------------------------------------------------------------------------------------------
		joga:
		switch(v2){
		case 0:
			g2i--;
			g2posy = g2i * q;
			ghost2 = al_load_bitmap("ghost.tga");
			//verificaMorto();
			v2 = 0;
			jogadaRealizada = true;
			break;
		case 1:
			g2i++;
			g2posy = g2i * q;
			ghost2 = al_load_bitmap("ghost.tga");
			//verificaMorto();
			v2=1;
			jogadaRealizada = true;
			break;
		case 2:
			g2j--;
			g2posx = g2j * q;
			ghost2 = al_load_bitmap("ghost.tga"); 
			//verificaMorto();
			v2=2;
			jogadaRealizada = true;
			break;
		case 3:
			g2j++;
			g2posx = g2j * q;
			ghost2 = al_load_bitmap("ghost.tga"); 
			//verificaMorto();
			v2=3;
			jogadaRealizada = true;
			break;		
		}
	}
}
void movimentaG3(){
	bool jogadaRealizada = false;
	bool jogadaPossivel = false;

	while(!jogadaRealizada){
		inicio:
		jogadaPossivel = possivel(v3, g3i, g3j);
		if(jogadaPossivel){
			goto joga;
		}else{
			v3 = rand() % 4;
			goto inicio;
		}
		// ---------------------------------------------------------------------------------------------
		joga:
		switch(v3){
		case 0:
			g3i--;
			g3posy = g3i * q;
			ghost3 = al_load_bitmap("ghost.tga");
			//verificaMorto();
			v3 = 0;
			jogadaRealizada = true;
			break;
		case 1:
			g3i++;
			g3posy = g3i * q;
			ghost3 = al_load_bitmap("ghost.tga");
			//verificaMorto();
			v3=1;
			jogadaRealizada = true;
			break;
		case 2:
			g3j--;
			g3posx = g3j * q;
			ghost3 = al_load_bitmap("ghost.tga"); 
			//verificaMorto();
			v3=2;
			jogadaRealizada = true;
			break;
		case 3:
			g3j++;
			g3posx = g3j * q;
			ghost3 = al_load_bitmap("ghost.tga"); 
			//verificaMorto();
			v3=3;
			jogadaRealizada = true;
			break;		
		}
	}
}
void movimentaG4(){
	bool jogadaRealizada = false;
	bool jogadaPossivel = false;
	while(!jogadaRealizada){
		inicio:
		jogadaPossivel = possivel(v4, g4i, g4j);
		if(jogadaPossivel){
			//goto joga;
		}else{
			v3 = rand() % 4;
			goto inicio;
		}
		// Tenho que arrumar um jeito de fazer o fantaasma achar o melhor caminho
		/* Possibilidades:
			- Tentar igualar o x com o do pacman por meio de um for (testando jogada por jogada) do x onde ele tá até o x do pacman e depois fazer o mesmo com o y.
			-
		
		 */
		//joga:

	}

}
//--------------------------------------------------------------------------------------------

int main(int argc, char** argv){
	if(!inicializa()) {
		return -1;
	}
	while(!sair && pontos != 2530) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			if(key[KEY_UP] && MAPA[i - 1][j] != '1') {
				i--;
				posy = i * q;
				pacman = al_load_bitmap("pacmanup.tga");
				jogada = true;
			}
			if(key[KEY_DOWN] && MAPA[i + 1][j] != '1') {
				i++;
				posy = i * q;
				pacman = al_load_bitmap("pacmandown.tga");
				jogada = true;
			}
			if(key[KEY_LEFT] && MAPA[i][j - 1] != '1') {
				j--;
				posx = j * q;
				pacman = al_load_bitmap("pacmanleft.tga");
				jogada = true;
			}
			if(key[KEY_RIGHT] && MAPA[i][j + 1] != '1') {
				j++;
				posx = j * q;
				pacman = al_load_bitmap("pacmanright.tga");
				jogada = true;
			}
			redraw = true;
			if(jogada == true){
				movimentaG1();
				movimentaG2();
				movimentaG3();
				//movimentaG4();
			}
		} else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		} else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {

			switch(ev.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					key[KEY_UP] = true;
					break;

				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = true;
					break;

				case ALLEGRO_KEY_LEFT:
					key[KEY_LEFT] = true;
					break;

				case ALLEGRO_KEY_RIGHT:
					key[KEY_RIGHT] = true;
					break;
			}
		} else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					key[KEY_UP] = false;
					break;

				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = false;
					break;

				case ALLEGRO_KEY_LEFT:
					key[KEY_LEFT] = false;
					break;

				case ALLEGRO_KEY_RIGHT:
					key[KEY_RIGHT] = false;
					break;

				case ALLEGRO_KEY_ESCAPE:
					sair = true;
					break;
			}
		}

		if(redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));
			carregarMapa();
			// al_draw_bitmap(mapa,0,0,0);
			al_draw_bitmap(pacman, posx, posy, 0);
			al_flip_display();
		}

	}

	if(pontos == 2530) {
		fonte = al_load_font("arial.ttf", 48, 0);
		al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W / 2, 215, ALLEGRO_ALIGN_CENTRE, "VOCE VENCEU!");
		al_flip_display();
		al_rest(5.0);
	} 

	// al_destroy_bitmap(mapa);
	al_destroy_bitmap(wall);
	al_destroy_bitmap(pacman);
	al_destroy_bitmap(pill);
	al_destroy_bitmap(ghost1);
	al_destroy_bitmap(ghost2);
	al_destroy_bitmap(ghost3);
	al_destroy_bitmap(ghost4);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}