#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <wait.h>

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
            outComm[argc++][&inComm[i] - argBeg] = 0;        // null ended string
            argBeg = &inComm[i + 1];
        }
    }
    memcpy(outComm[argc], argBeg, &inComm[i] - argBeg );
    outComm[argc][&inComm[i] - argBeg] = 0;        // null ended string
    outComm[++argc] = NULL;
}

void recBash(int cnt1, int comMax)
{
    int fd[2];
    int cnt = cnt1;
    char comParseAr[8][16];
    char *a[8];
    for (int i = 0; i < 8; i++)
        a[i] = comParseAr[i];

    if(cnt == comMax) // last comm
    {
        // last comm - pipe it to result.out
//            freopen ("/home/box/result.out", "w", stdout);
        freopen ("/home/genius/result.out", "w", stdout);
    }

    if(cnt-- <= 1)
    {
        // first comm
        parseCom(singleComm[cnt], a);
        execvp(a[0], a);
    }

    pipe(fd);
    if(int res = fork())
    {
        // parent
        close(fd[FD_WR]);
        close(STDIN_FILENO);
        dup2(fd[FD_RD], STDIN_FILENO); // pipe wr end = stdout
        close(fd[FD_RD]);

        parseCom(singleComm[cnt], a);
        execvp(a[0], a);
    }
    else if(res == 0)
    {
        // child
        close(fd[FD_RD]);
        close(STDOUT_FILENO);
        dup2(fd[FD_WR], STDOUT_FILENO); // pipe wr end = stdout
        close(fd[FD_WR]);

        recBash(cnt, comMax);
    }
    else return;    // err
}

int main()
{
//    remove("/home/box/result.out");
    char comm[256] = "";
    if( fgets(comm, sizeof(comm), stdin) )
    {
        int n = 0;
        int comCnt = 0;
        for(int i=0; comm[i] && (comm[i] != '\n') && (comm[i] != '\r'); i++)
        {
            if(comm[i] != '|')
            {
                singleComm[comCnt][n++] = comm[i];
            }
            else
            {
                singleComm[comCnt][n] = 0;
                if(singleComm[comCnt][n - 1] == ' ') singleComm[comCnt][--n] = 0;   // delete spaces before |
                while(comm[i + 1] == ' ') i++;                                      // delete spaces after |
                n = 0;
                comCnt++;
            }
        }
        singleComm[comCnt][n] = 0;
        if(singleComm[comCnt][n - 1] == ' ') singleComm[comCnt][--n] = 0;
        comCnt++;

        recBash(comCnt, comCnt);
    }

    return 0;
}

