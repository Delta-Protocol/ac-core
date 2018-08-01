#include <vector>
#include <iostream>

/*

  This is an implementation of the Tromino Tiling Algorithm, based on the c implementation 
  found here https://github.com/ambarmodi/Tromino-Tiling-Algorithm

*/

class  TrominoTile
{

public:

    TrominoTile(int k,int m_hr,int m_hc)
        : m_size {1<<k},
          m_hr {m_hr},
          m_hc {m_hc},
          board (m_size,std::vector<int>(m_size,0))
    {
        board[m_hr][m_hc]=-1;
    }
   
    void trominoTile() 
    {
        recursionTrominoTile(m_size,0,0);
    }

    friend std::ostream& operator<<(std::ostream& os, const TrominoTile& data)
    {
        for (int i = 0 ; i < data.m_size ; i++){
            for(int j = 0; j < data.m_size ; j++ ) {

                os << ' ' << data.board[i][j];

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
        int i,j;

        if(n == 2) {

            m_cnt++;

            for( i = 0 ;i < n ; i++) {

                for( j = 0 ; j < n ; j++){

                    int tmp_x {x+i}, tmp_y{y+j};

                    if((tmp_x < 0  || tmp_y < 0 || tmp_x >= m_size || tmp_y >=m_size ||tmp_y >= m_size) && board[tmp_x][tmp_y] != 0 ) return ;

                    board[tmp_x][tmp_y] = m_cnt;
                }
            }
            return ;
        }

        //If missing Tile is in 1st quardent
        if( m_hr< x + n/2 && m_hc < y+ n/2) {

            putTromino(x + n/2,
                       y + (n/2) - 1,
                       x + n / 2,
                       y + n / 2,
                       x + n / 2-1,
                       y + n / 2 );
        }

        //If missing Tile is in 2st quardent
        else if( m_hr >= x + n/2 && m_hc < y + n/2) {

            putTromino(x + n/2,
                       y + (n/2) - 1,
                       x + n/2,
                       y + n/2,
                       x + n/2-1,
                       y + n/2-1);
        }

        //If missing Tile is in 3st quardent
        else if(m_hr < x + n/2 && m_hc >= y + n/2) {

            putTromino(x + (n/2) - 1,
                       y + (n/2),
                       x + (n/2),
                       y + n/2,
                       x + (n/2)-1,
                       y + (n/2) -1);
        }

        //If missing Tile is in 4st quardent
        else if(m_hr >= x + n/2 && m_hc >= y + n/2) {

            putTromino(x + (n/2) -1, 
                       y + (n/2),
                       x + (n/2),
                       y + (n/2) -1,
                       x + (n/2)-1,
                       y + (n/2)-1);
        }

        recursionTrominoTile( n/2, 
                              x, 
                              y + n/2 );

        recursionTrominoTile(n/2, 
                             x, 
                             y);

        recursionTrominoTile(n/2, 
                             x+n/2, 
                             y);

        recursionTrominoTile(n/2, 
                             x+n/2, 
                             y+n/2);

    }

    void putTromino(int x1,int y1,int x2,int y2,int x3, int y3) 
    {
        m_cnt++;

        if( x1 < 0 || y1 < 0 || x2 < 0   || 
            y2 < 0 || x3 < 0 || y3 < 0   ||
            x1 >= m_size || y1 >= m_size || 
            x2 >= m_size || y2 >= m_size || 
            x3 >= m_size || y3 >= m_size) return ; 

        board[x1][y1] = m_cnt;

        board[x2][y2] = m_cnt;

        board[x3][y3] = m_cnt;

    }

    int m_size;
    int m_cnt{0};
    int m_hr;
    int m_hc;
    std::vector<std::vector<int>>  board;
};

int main(int argc,char ** argv)
{

    TrominoTile tromino_tile(4,0,0);

    tromino_tile.trominoTile();

    std::cout<<tromino_tile;

}
