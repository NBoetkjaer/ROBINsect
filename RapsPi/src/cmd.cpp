#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <stropts.h>
#include <asm-generic/ioctls.h>
#include <unistd.h>


// _kbhit() detects if a keystroke is available for getch().
// http://www.flipcode.com/archives/_kbhit_for_Linux.shtml

/**
Linux (POSIX) implementation of _kbhit().
Morgan McGuire, morgan@cs.brown.edu
*/
int _kbhit()
{
	static const int STDIN = 0;
	static bool initialized = false;
	if (! initialized)
	{
		// Use termios to turn off line buffering
		termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, NULL);
		initialized = true;
	}
	
	int bytesWaiting;
	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}

/* reads from keypress, doesn't echo */
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}