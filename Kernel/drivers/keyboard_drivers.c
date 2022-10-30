#include <keyboard_driver.h>
#include <video_driver.h>
#define MAX_SCANCODE 58

#define ENGLISH 0
#define SPANISH 1

static int language = SPANISH;

//estan cargados los teclados en ingles y español
static char keyboards[2][MAX_SCANCODE][2] ={
    {
    {   0,0   },
    {ESC , ESC}, 
    { '1','!' }, 
    { '2','@' },
    { '3','#' },
    { '4','$' }, 
    { '5','%' }, 
    { '6','^' },
    { '7','&' },
    { '8','*' }, 
    { '9','(' }, 
    { '0',')' },
    { '-','_' },
    { '=','+' }, 
    {'\b','\b'}, 
    {'\t','\t'},
    { 'q','Q' },
    { 'w','W' }, 
    { 'e','E' }, 
    { 'r','R' },
    { 't','T' },
    { 'y','Y' }, 
    { 'u','U' }, 
    { 'i','I' },
    { 'o','O' },
    { 'p','P' }, 
    { '[','{' }, 
    { ']','}' },
    {'\n','\n'},
    {   0,0   }, 
    { 'a','A' }, 
    { 's','S' },
    { 'd','D' },
    { 'f','F' }, 
    { 'g','G' }, 
    { 'h','H' },
    { 'j','J' },
    { 'k','K' }, 
    { 'l','L' }, 
    { ';',':' },
    {'\'','\"'},
    { '`','~' }, 
    {   0,0   }, 
    { '\\','|'},
    { 'z','Z' },
    { 'x','X' }, 
    { 'c','C' }, 
    { 'v','V' },
    { 'b','B' },
    { 'n','N' }, 
    { 'm','M' }, 
    { ',','<' },
    { '.','>' },
    { '/','?' }, 
    {   0,0   }, 
    {   0,0   },
    {   0,0   },
    { ' ',' ' }    
    },
    {
    {   0,0   },
    {ESC , ESC}, 
    { '1','!' }, 
    { '2','"' },
    { '3','#' },
    { '4','$' }, 
    { '5','%' }, 
    { '6','&' },
    { '7','/' },
    { '8','(' }, 
    { '9',')' }, 
    { '0','=' },
    { '\'','?'},
    { '¿','¡' }, 
    {'\b','\b'}, 
    {'\t','\t'},
    { 'q','Q' },
    { 'w','W' }, 
    { 'e','E' }, 
    { 'r','R' },
    { 't','T' },
    { 'y','Y' }, 
    { 'u','U' }, 
    { 'i','I' },
    { 'o','O' },
    { 'p','P' }, 
    { '`','\"' }, 
    { '+','*' },
    {'\n','\n'},
    {   0,0   }, 
    { 'a','A' }, 
    { 's','S' },
    { 'd','D' },
    { 'f','F' }, 
    { 'g','G' }, 
    { 'h','H' },
    { 'j','J' },
    { 'k','K' }, 
    { 'l','L' }, 
    { '~'+1,'~' },
    {'{','['},
    { '`','~' }, 
    {   0,0   }, 
    { '}',']'},
    { 'z','Z' },
    { 'x','X' }, 
    { 'c','C' }, 
    { 'v','V' },
    { 'b','B' },
    { 'n','N' }, 
    { 'm','M' }, 
    { ',',';' },
    { '.',':' },
    { '-','_' }, 
    {   0,0   }, 
    {   0,0   },
    {   0,0   },
    { ' ',' ' }    
}};

static int shiftActivated = 0;
static int capsActivated = 0;

#define MAX_BUFFER 1000
static char keyBuffer[MAX_BUFFER];
static int bufferCount = 0;

void clearKeyBuffer ( void );

//saca el primero que esta en el vector
static char popBuffer(){
    char key = keyBuffer[0];
    for(int i = 0; i < bufferCount; i++){
        keyBuffer[i] = keyBuffer[i+1];
    }
    bufferCount--;
    return key;
}
/**
 * 
 * @return devuelve el la primer tecla presionada
 */
char getKey(){
    if(bufferCount <= 0)
        return 0;
    
    return popBuffer();
}
/**
 * @return el ultimo caracter presionado
 */
char getLastChar(){
    return keyBuffer[bufferCount-1];
}


static int isValidScancode(int scancode){
    return scancode > 0 && scancode < MAX_SCANCODE;
}

static int isLetter(int scancode){
    if(scancode == 0x27)
        return 1;
    char ascii = keyboards[language][scancode][0];
    return  (ascii >= 'a' && ascii <= 'z');
}
#define SCANCODE_UP_ARROW 72
#define SCANCODE_DOWN_ARROW 80
#define SCANCODE_LEFT_ARROW 75
#define SCANCODE_RIGHT_ARROW 77
#define ASC_UP    '\200'
#define ASC_DOWN  '\201'
#define ASC_LEFT  '\202'
#define ASC_RIGHT '\203'
#define LSHIFT_PRESSED 0x2A          /* Scan codes for shift press, */
#define LSHIFT_RELEASED 0xAA        /* shift release and capslock  */
#define RSHIFT_PRESSED 0x36        /* keys.                       */
#define RSHIFT_RELEASED 0xB6
#define CAPSLOCK 0x3A
#define CAPSLOCK_RELEASED 0xBA 

/**
 * @brief guarda el caracter q se presiono en el buffer 
 */
void storeKey(){

    int scancode  = inb(0x60);
    
    
    if(scancode == LSHIFT_PRESSED || scancode == RSHIFT_PRESSED){
        shiftActivated = 1;
    }
    
    else if(scancode == LSHIFT_RELEASED || scancode == RSHIFT_RELEASED){
        shiftActivated = 0;
    }
    else if(scancode == CAPSLOCK){
        capsActivated = !capsActivated;
    }
    if(isValidScancode(scancode) && bufferCount < MAX_BUFFER){
        if( isLetter(scancode) && capsActivated ){
            keyBuffer[bufferCount] = keyboards[language][scancode][1];
        }
        else{
            keyBuffer[bufferCount] = keyboards[language][scancode][shiftActivated ? 1: 0];   
        }
        bufferCount++;
        return;
    }
    if(scancode == SCANCODE_UP_ARROW){
        keyBuffer[bufferCount] = ASC_UP;
        bufferCount++;
        return;
    }
    if(scancode == SCANCODE_DOWN_ARROW){
        keyBuffer[bufferCount] = ASC_DOWN;
        bufferCount++;
        return;
    }
    if(scancode == SCANCODE_LEFT_ARROW){
        keyBuffer[bufferCount] = ASC_LEFT;
        bufferCount++;
        return;
    }
    if(scancode == SCANCODE_RIGHT_ARROW){
        keyBuffer[bufferCount] = ASC_RIGHT;
        bufferCount++;
        return;
    }
    return;
    
}





void clearKeyBuffer(){
	for ( int i = 0; i < MAX_BUFFER; i++ )
		keyBuffer[i] = (char)0;
	bufferCount= 0;
}

void changeLanguage(int lan){
    language = lan;
}





















