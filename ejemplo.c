#include <gb/gb.h>
#include <stdio.h>
#include <time.h>
#include "Tile1.c"

#include "mapa.c"
#include "Nivel1.c"

#include "titlescreen.c"
#include "pantalla.c"

#include "fin.c"
#include "gameover.c"

//DECLARAR VARIABLES
unsigned char plx, ply;
unsigned char direction = 0; // 0 = derecha, 1 = izquierda, 2 = arriba, 3 = abajo
unsigned char direction_bala = 0;
unsigned char direccion_enemigo = 0;  // 0 = derecha, 1 = izquierda, 2 = arriba, 3 = abajo
unsigned char detectar_colision(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2) {
    return (x1 < x2 + 8 && x1 + 8 > x2 && y1 < y2 + 8 && y1 + 8 > y2);
}
unsigned int game_over = 0;
unsigned int score;

//SE CREAN LAS VARIABLES MENSAJE Y NUM PARA LA PUNTUACION 
unsigned char Mensaje[] =
    {
        20,21,22,23,24,25
    };

unsigned char Numeros[] =
    {
        26,27,28,29,30,31,32,33,34,35
    };


struct Bullet {
    unsigned char x;
    unsigned char y;
    unsigned char active;
};

struct Bullet bullet;


struct Enemigo {
    unsigned char x;
    unsigned char y;
    unsigned char active;
};

struct Enemigo enemigo;


//MUESTRA EL TITTLE SCREEN
void pantalla_inicio() {
  
    printf("\n\n\n\n");
    printf("     GANK TANK\n");
    printf("\n\n\n\n");
    printf(" Press Start\n");
    printf("\n");

    set_bkg_data(0,36,titlelabel);
    set_bkg_tiles(0,0,20,20,PantallaLabel);

    waitpad(J_START);       
}

// MUESTRA EL GAME OVER 
void mostrar_game_over() {
   
    HIDE_SPRITES;
    move_sprite(0, 0, 0);
    move_sprite(1, 0, 0);
    move_sprite(2, 0, 0);
    set_bkg_data(0,20,finLabel);
    set_bkg_tiles(0,0,20,20,gameoverLabel);

    waitpad(J_START);
        SHOW_SPRITES;
        plx = 60;
        ply = 70;

        direction = 0;
        direction_bala = 0;
        direccion_enemigo = 0;
        
        game_over = 0;
        score = 0;
        enemigo.active = 0;
        move_sprite(2, 0, 0); 
        set_bkg_data(0,7,mapalabel);            
        set_bkg_tiles(0,0,20,20,Nivel1Label);
    

}

//CREA UNA VENTANA CON LOS TILES DE LAS VARIABLES ANTERIORES 
void acutalizar_score()
{
    set_win_tiles(7,1,1,1,Numeros+((score/10))%10);
    set_win_tiles(8,1,1,1,Numeros+(score%10));
}

// VOID MAIN 
void main()
{
    // LLAMA A LA PANTALLA PRINCIPAL
    pantalla_inicio();
    plx = 60;
    ply = 70;

    //DEFINE EL SONIDO
    NR52_REG = 0X80;
    NR50_REG = 0X77;
    NR51_REG = 0XFF;


// DEFINE LOS TILESY MUESTRA
    set_sprite_data(0, 21, TileLabel);      
    set_sprite_tile(0, 0);

    SHOW_SPRITES;
    SHOW_WIN;

    // DEFINE MAPA YY SUS TILES
    set_bkg_data(0,7,mapalabel);            
    set_bkg_tiles(0,0,20,20,Nivel1Label);

    set_win_tiles(1,1,8,1,Mensaje);
    move_win(7,120);

    //INICIA EL BUCLE
    while(1)
    {

    // LLAMA AL ACTUALIZADOR DE PUNTOS
        acutalizar_score();

    // MOVIMIENTO PERSONAJE
        if (joypad() & J_LEFT)
        {
            --plx;
            direction = 1;
             

        }
        if (joypad() & J_RIGHT)
        {
            ++plx;
            direction = 0;
           
        }
        if (joypad() & J_UP)
        {
            --ply;
            direction = 2;
           
        }
        if (joypad() & J_DOWN)
        {
            ++ply;
            direction = 3;    
        }
        
     
        //creación enemigo 
        if (!enemigo.active & ply < 80) {
            enemigo.x = plx;
            enemigo.y = ply+40;
            enemigo.active = 1;         
        } 
        if (!enemigo.active & ply > 80) {
            enemigo.x = plx;
            enemigo.y = ply-40;
            enemigo.active = 1;         
        } 

        //movimiento enemigo
       else{
            //Direccion a tomar?
             if (enemigo.x > 140){
                    direccion_enemigo = 1;
            }
            else if (enemigo.x < 20){
                    direccion_enemigo = 0;
            }
            
            //Preguntamos la direccion 
            if (direccion_enemigo == 0){
                set_sprite_tile(2, 6);
                enemigo.x +=1; //velocidad bala??
            }
            else{
                set_sprite_tile(2, 5);
                enemigo.x -=1;    
            }
             move_sprite(2, enemigo.x, enemigo.y);
        }
        
    
        // disparar bala 
        if (joypad() & J_A && !bullet.active) {
            bullet.x = plx ;
            bullet.y = ply ;
            bullet.active = 1;
            direction_bala=direction;
            set_sprite_tile(0, 1); // define el sprite de la bala 
            
            //REPRODUCE EL SONIDO
            NR10_REG = 0x16;
            NR11_REG = 0x40;
            NR12_REG = 0x73;
            NR13_REG = 0x00;
            NR14_REG = 0xC3;
        }

        // Mover bala
        if (bullet.active) {
            if (direction_bala == 0) {
                bullet.x += 4;
            } else if (direction_bala == 1) {
                bullet.x -= 4;
            } else if (direction_bala == 2) {
                bullet.y -= 4;
            } else if (direction_bala == 3) {
                bullet.y += 4;
            }

            move_sprite(1, bullet.x, bullet.y);
            
            // Desactivar si toca borde pantalla
            if (bullet.x < 0 || bullet.x > 160 || bullet.y < 0 || bullet.y > 160) {
                bullet.active = 0;
                move_sprite(1, 0, 0);
            }   
        }

        // Verifica si la bala ha colisionado con el enemigo
        if (bullet.active && enemigo.active && detectar_colision(bullet.x, bullet.y, enemigo.x, enemigo.y)) {
            bullet.active = 0;
            move_sprite(1, 0, 0);
            enemigo.active = 0;
            move_sprite(2, 0, 0);
            ++score;
        }

        // Detecta colision del tanque con el enemigo
        if (enemigo.active && detectar_colision(plx, ply, enemigo.x, enemigo.y)) {
            enemigo.active = 0;
            game_over = 1;
        }

        //si se pierde la partida (1 == perder) llama al void game_over
        if (game_over == 1) {
            mostrar_game_over();
            
        }
        
         // Cambiar la animación del sprite según la dirección
        switch (direction) {
            case 0: // Derecha
                set_sprite_tile(0, 2);
                break;
            case 1: // Izquierda
                set_sprite_tile(0, 4);
                break;
            case 2: // Arriba
                set_sprite_tile(0, 1);
                break;
            case 3: // Abajo
                set_sprite_tile(0, 3);
                break;
        }

        //dibuja el tanque mio
        move_sprite(0, plx, ply);
        delay(20);
    
    }

}





