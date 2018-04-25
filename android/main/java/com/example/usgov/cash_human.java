package com.example.usgov;

public class cash_human {

    static String show(String raw) {

        Long l=Long.parseLong(raw);
        String s=l.toString();
        if (s.length()<8) {
            for (int i=s.length(); i<8; ++i) s='0'+s;
            s="0."+s;
        }
        else {
            s=s.substring(0,s.length()-8)+'.'+s.substring(s.length()-8,s.length());
        }
        return s;
    }

};



