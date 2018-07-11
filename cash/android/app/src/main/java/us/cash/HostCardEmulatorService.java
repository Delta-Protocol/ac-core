package us.cash;

import android.content.Intent;
import android.nfc.cardemulation.HostApduService;
import android.os.Binder;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

import java.util.Arrays;

public class HostCardEmulatorService extends HostApduService {

    public HostCardEmulatorService() {

    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "onCreate");

        //mIntent = new Intent(this, MyActivity.class);
        //mIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        //startActivity(mIntent);
    }


    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        // Check if intent has extras
        if(intent.getExtras() != null){

            // Get message
            message = intent.getExtras().getString("message");
            Log.d("INTENT", message);
        }

        return START_NOT_STICKY;
    }

    public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

    public static String ByteArrayToHexString(byte[] bytes) {
        final char[] hexArray = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
        char[] hexChars = new char[bytes.length * 2]; // Each byte has two hex characters (nibbles)
        int v;
        for (int j = 0; j < bytes.length; j++) {
            v = bytes[j] & 0xFF; // Cast bytes[j] to int, treating as unsigned value
            hexChars[j * 2] = hexArray[v >>> 4]; // Select hex character from upper nibble
            hexChars[j * 2 + 1] = hexArray[v & 0x0F]; // Select hex character from lower nibble
        }
        return new String(hexChars);
    }

    public static String TAG = "Host Card Emulator";
    public static byte STATUS_SUCCESS[]=hexStringToByteArray("9000");
    public static byte STATUS_FAILED[]=hexStringToByteArray("6F00");
    public static byte CLA_NOT_SUPPORTED[]=hexStringToByteArray("6E00");
    public static byte INS_NOT_SUPPORTED[]=hexStringToByteArray("6D00");
    public static byte AID[]=hexStringToByteArray("F000016672"); //https://stackoverflow.com/questions/27877373/how-to-get-aid-for-reader-host-based-card-emulation
    public static byte INS = (byte)0xA4;
    public static byte CLA = (byte)0x00;
    public static byte LE = (byte)0x00;
    public static int MIN_APDU_LENGTH = 10;


    @Override
    public void onDeactivated(int reason) {
        Log.d(TAG, "Deactivated: " + reason);
    }


    boolean checkAID(byte[] commandApdu) {
        for (int i=0; i<AID.length; ++i) {
            if (commandApdu[5+i] != AID[i]) return false;
        }
        return true;
    }
/*
    String parseAddr(byte[] commandApdu) {
        byte l=commandApdu[4];
        l-=AID.length;
        Log.d("XXXXXXXXXXX", ""+l);
        String addr=new String();
        for (int i=0; i<l; ++i) {
            addr+=commandApdu[5+AID.length+i];
        }
        return addr;
    }
*/
    String message=""; //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 10000 PAY";


    @Override
    public byte[] processCommandApdu(byte[] commandApdu, Bundle extras) {
        Log.d(TAG, "processCommandApdu ");
        if (commandApdu == null) {
            return STATUS_FAILED;
        }

        //val hexCommandApdu = Utils.toHex(commandApdu)
        if (commandApdu.length < MIN_APDU_LENGTH) {
            return STATUS_FAILED;
        }

        if (commandApdu[0] != CLA) {
            return CLA_NOT_SUPPORTED;
        }
        if (commandApdu[1] != INS) {
            return CLA_NOT_SUPPORTED;
        }

        if (!checkAID(commandApdu)) return STATUS_FAILED;

        //String addr=parseAddr(commandApdu);

//        Log.d(TAG, "address "+addr);

        return concatArrays(message.getBytes(),STATUS_SUCCESS);
/*
        if (aid == AID)  {
        } else {
            return Utils.hexStringToByteArray(STATUS_FAILED)
        }
*/
    }

    public static byte[] concatArrays(byte[] first, byte[]... rest) {
        int totalLength = first.length;
        for (byte[] array : rest) {
            totalLength += array.length;
        }
        byte[] result = Arrays.copyOf(first, totalLength);
        int offset = first.length;
        for (byte[] array : rest) {
            System.arraycopy(array, 0, result, offset, array.length);
            offset += array.length;
        }
        return result;
    }


}
