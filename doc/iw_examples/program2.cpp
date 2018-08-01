#include <vector>
#include <iostream>
#include <iomanip>

/*

  This is an implementation of the Tromino Tiling Algorithm, based on the c implementation 
  found here https://github.com/ambarmodi/Tromino-Tiling-Algorithm, 

  note that c program had a bug in recursionTrominoTile function, which is fixed in this program

*/

class  TrominoTile
{

public:

    TrominoTile(int k,int hr,int hc)
        : m_size {1<<k},
          board (m_size,std::vector<int>(m_size,0))
    {
        board[hr][hc]=-1;
    }
   
    void trominoTile() 
    {
        recursionTrominoTile(m_size,0,0);
    }

    friend std::ostream& operator<<(std::ostream& os, const TrominoTile& data)
    {
        for (int i=0 ; i < data.m_size ; i++){
            for(int j=0;j < data.m_size ; j++ ) {
                os << ' ' << std::setw(2)<<std::setfill('0')<<data.board[i][j];
            }
            os << '\n';
        }
        return os;
    }

private:
    /*
    * TrominoTile Recursive function
    */

    void recursionTrominoTile(int n,int x,int y) 
    {
        int hr,hc;
        if(n == 2){
            m_cnt++;
            for(int i=0;i<n;i++) {
                for(int j=0;j<n;j++){
                    if(board[x+i][y+j]==0)
                        board[x+i][y+j] = m_cnt;
                }
            }
            return ;
        }

        for(int i=x;i<n;i++) {
            for(int j=y;j<board[i].size();j++){
                if(board[i][j] != 0 ) {
                    hr=i; hc=j;
                }
            }
        }

        //If missing Tile is in 1st quardent
        if(hr< x + n/2 && hc < y+ n/2) {
            putTromino(x+n/2,y+(n/2)-1,x+n/2,y+n/2,x+n/2-1,y+n/2);
        }
        //If missing Tile is in 2st quardent
        else if(hr>=x+ n/2 && hc < y + n/2) {
            putTromino(x+n/2-1,y+(n/2)-1,x+n/2-1,y+n/2,x+n/2,y+n/2);
        }
        //If missing Tile is in 3st quardent
        else if(hr < x + n/2 && hc >= y + n/2) {
            putTromino(x+(n/2) - 1,y+ (n/2)-1,x+(n/2),y+n/2-1,x+(n/2),y+(n/2));
        }

        //If missing Tile is in 4st quardent
        else if(hr >= x + n/2 && hc >= y + n/2) {
            putTromino(x+(n/2) -1, y+ (n/2) -1,x+(n/2),y+(n/2) -1,x+(n/2),y+(n/2)-1);
        }
        recursionTrominoTile(n/2, x, y+n/2);
        recursionTrominoTile(n/2, x, y);
        recursionTrominoTile(n/2, x+n/2, y);
        recursionTrominoTile(n/2, x+n/2, y+n/2);
    }

    void putTromino(int x1,int y1,int x2,int y2,int x3, int y3) 
    {
        m_cnt++;
        board[x1][y1] = m_cnt;
        board[x2][y2] = m_cnt;
        board[x3][y3] = m_cnt;
    }

    int m_size;
    int m_cnt{0};
    std::vector<std::vector<int>>  board;
};

int main(int argc,char ** argv)
{

    TrominoTile tromino_tile(4,0,0);
    tromino_tile.trominoTile();
    std::cout<<tromino_tile;

}
