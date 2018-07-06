package us.cash;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Movie;
import android.view.View;

import java.io.InputStream;

public class gifView extends View {
    Movie movie,movie1;
    InputStream is=null,is1=null;
    long moviestart;
    public gifView(Context context) {
        super(context);
        is = context.getResources().openRawResource(R.drawable.carlton);
        movie = Movie.decodeStream(is);
    }
    @Override
    protected void onDraw(Canvas canvas) {
        canvas.drawColor(Color.WHITE);
        super.onDraw(canvas);
        long now=android.os.SystemClock.uptimeMillis();
        System.out.println("now="+now);
        if (moviestart == 0) { // first time
            moviestart = now;

        }
        System.out.println("dur="+movie.duration());
        System.out.println("\tmoviestart="+moviestart);
        int relTime = (int)((now - moviestart));
        if (movie.duration()>0) relTime%=movie.duration();
        System.out.println("time="+relTime+"\treltime="+movie.duration());
        movie.setTime(relTime);
        movie.draw(canvas,this.getWidth()/2-20,this.getHeight()/2-40);
        this.invalidate();
    }

}
