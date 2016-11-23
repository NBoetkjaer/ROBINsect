/***********************************************************************
 * VT100 CODES
 ***********************************************************************/

#define ESCAPE "\x1b"

#define BLACK   0
#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7
#define DEFAULT_COLOR 9 // Whatever the terminal prefers

#define STRINGIFY(x) #x
#define FOREGROUND(x) ESCAPE "[3" STRINGIFY(x) "m"
#define BACKGROUND(x) ESCAPE "[4" STRINGIFY(x) "m"

#define UNDERLINE ESCAPE "[4m"
#define REVERSE ESCAPE "[7m"
#define NORMAL ESCAPE "[0m"

#define HOME ESCAPE "[H"
#define UP11 ESCAPE "[11A"
#define DOWN ESCAPE "[22B"
#define SAVE_POS ESCAPE "[s"
#define RESTORE_POS ESCAPE "[u"

#define ERASE_LINE ESCAPE "[2K"
#define ERASE_TO_END ESCAPE "[K"
#define ERASE_TO_BOTTOM ESCAPE "[J"
#define ERASE_ALL ESCAPE "[2J"

#define NEWLINE "\r\n" ERASE_LINE

int _kbhit();
int getch(void);

class cmd
{
public:
	
private:

};