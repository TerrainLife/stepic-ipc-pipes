#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

static const unsigned int FD_RD = 0;
static const unsigned int FD_WR = 1;

char singleComm[8][32] = {"","","","","","","",""};

void parseCom(char *inComm, char **outComm)
{
    char *argBeg = inComm;
    int i = 0;
    int argc = 0;
    for(; inComm[i]; i++)
    {
        if(inComm[i] == ' ')
        {
            memcpy(outComm[argc], argBeg, &inComm[i] - argBeg );
            outComm[argc++][&inComm[i] - argBeg - 1] = 0;        // null ended string
            cout << "COM = " << outComm[argc - 1] << strlen(outComm[argc - 1]) << endl;
            argBeg = &inComm[i + 1];
        }
    }
    memcpy(outComm[argc], argBeg, &inComm[i] - argBeg );
    cout << "COM0 = " << &inComm[i] - argBeg << endl;
    outComm[argc][&inComm[i] - argBeg - 1] = 0;        // null ended string
    cout << "COM1 = " << outComm[argc] << strlen(outComm[argc]) << endl;
    printf("Chars = %d   %c %c %c %c", strlen(outComm[argc]), outComm[argc][0],outComm[argc][1], outComm[argc][2], outComm[argc][3]);
    outComm[++argc] = NULL;
}

void recBash(int cnt, int comMax)
{
    int fd[2];
    char comParseAr[8][16];
    char *a[8];
    for (int i = 0; i < 8; i++)
        a[i] = comParseAr[i];

    if(cnt == (comMax - 1))
    {
        // last comm - pipe it to result.out
//        int outFd = open("/home/box/result.out", O_WRONLY | O_CREAT);
//        close(fd[FD_RD]);
//        close(STDOUT_FILENO);
//        dup2(fd[FD_WR], STDOUT_FILENO); // pipe wr end = stdout
//        close(fd[FD_WR]);
//        execlp(singleComm[cnt], singleComm[cnt], NULL);

//cout << singleComm[cnt] << strlen(singleComm[cnt]) << endl;

//        freopen ("/home/box/result.out", "w", stdout);
        freopen ("/home/genius/result.out", "w", stdout);
//        execlp(singleComm[cnt], singleComm[cnt], NULL);
        parseCom(singleComm[cnt], a);
        execvp(a[0], a);
//        execlp("who", "who", NULL);

        return;
    }

    pipe(fd);
    if(int res = fork())
    {
        // parent
        close(fd[FD_RD]);
        close(STDOUT_FILENO);
        dup2(fd[FD_WR], STDOUT_FILENO); // pipe wr end = stdout
        close(fd[FD_WR]);
        parseCom(singleComm[cnt], a);
        execvp(a[0], a);

        return;
    }
    else if(res == 0)
    {
        // child
        close(fd[FD_WR]);
        close(STDIN_FILENO);
        dup2(fd[FD_RD], STDIN_FILENO); // pipe wr end = stdout
        close(fd[FD_RD]);

        recBash(++cnt, comMax);
    }
    else return;    // err
}

int main()
{
    char comm[256] = "";
    if( fgets(comm, sizeof(comm), stdin) )
    {
//        cout << comm << endl;
        int n = 0;
        int comCnt = 0;
        for(int i=0; comm[i]; i++)
        {
            if(comm[i] != '|')
            {
                singleComm[comCnt][n++] = comm[i];
            }
            else
            {
                singleComm[comCnt][n] = 0;
                i++;
                n = 0;
                comCnt++;
            }
        }
        singleComm[comCnt][n] = 0;
        comCnt++;

        recBash(0, comCnt);
    }

    return 0;
}

