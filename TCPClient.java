import java.io.*; 
import java.net.*; 
class TCPClient {  
	public static void main(String argv[]) throws Exception  
	{   
		String hostName = argv[0];
		int portNumber = Integer.parseInt(argv[1]);

		String sentence;   
		String modifiedSentence;   
		
		BufferedReader inFromUser = new BufferedReader( new InputStreamReader(System.in));   
		Socket clientSocket = new Socket(hostName, 30020);   
		DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream());   
		BufferedReader inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));   
		sentence = inFromUser.readLine();   
		outToServer.writeBytes(sentence + '\n');   
		modifiedSentence = inFromServer.readLine();   
		System.out.println("FROM SERVER: " + modifiedSentence);   
		clientSocket.close();  
	} 
}
