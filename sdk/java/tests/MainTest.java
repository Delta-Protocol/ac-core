package us.test;

public class MainTest{

    public static void main(String [ ] args)  {

        try{
            SymmetricEncryptionTests symmetricEncryptionTests = new SymmetricEncryptionTests();
            EllipticCryptographyTests ellipticCryptographyTests = new EllipticCryptographyTests();
            ellipticCryptographyTests.testEllipticCryptography();
            symmetricEncryptionTests.test_symmetric_encryption();
            

            System.out.println("Java tests PASSED");
        }
        catch(Exception e){
            System.out.println("Java tests FAILED:" + e);
        }

        
    }
}