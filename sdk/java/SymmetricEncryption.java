import java.security.SecureRandom;
import java.security.Security;
import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.PBEKeySpec;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.util.Arrays;
import com.sun.org.apache.xml.internal.security.utils.Base64;

public class SymmetricEncryption {

    private static final SecureRandom random = new SecureRandom();
    private static final int iv_size = 12;
    private Cipher cipher = null;
    private SecretKey key = null;
    private byte[] iv = null;

    public SymmetricEncryption(SecretKey privA, ECPoint pubB) throws Exception {
        Security.addProvider(new BouncyCastleProvider());
        cipher = Cipher.getInstance("AES/GCM/NoPadding", "BC");
        iv = new byte[iv_size];
        EllipticCryptography ellipticCryptography = EllipticCryptography.getInstance();
        key = ellipticCryptography.generateSharedKey(privA,pubB);
    }
  
    public byte[] encrypt(byte[] plaintext) throws Exception {
        
        random.nextBytes(iv);
        cipher.init(Cipher.ENCRYPT_MODE, key, new IvParameterSpec(iv), random);
        return Arrays.concatenate(cipher.doFinal(plaintext), iv);
    }

    public byte[] decrypt(byte[] encrypted) throws Exception {
        
        iv = Arrays.copyOfRange(encrypted, encrypted.length - iv_size, encrypted.length);
        cipher.init(Cipher.DECRYPT_MODE, key, new IvParameterSpec(iv), random);
        return cipher.doFinal(Arrays.copyOfRange(encrypted, 0, encrypted.length - iv_size));
    }

}