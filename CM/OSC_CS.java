package controlmodule.motioncomposer;

import java.awt.Desktop;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PipedOutputStream;
import java.io.PrintStream;
//import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
//import java.net.SocketException;
import java.nio.file.Paths;
import java.util.ArrayList;
//import java.util.Date;
import java.util.HashMap;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JTextArea;

import osc.netutil.*;


public class OSC_CS {
	
	OSC_CS(){
		
	}
	static List<String> csList = new ArrayList<String>();
	static List<String> oscMessageList = new ArrayList<String>(); 
	static String strFileName = "CM/includes/config_.txt";
	static String strProcess="";
	static String strOldValue;
	static String strNewValue;
	static String[] strKeyValuePairs = null;
	static String strLine = null;
	static InetAddress senderIntAdd=null;
	static InetAddress receiverIntAdd=null;
	static int intSenderPort=0;
	static int inReceiverPort = 0;
	static InetSocketAddress CMtoTM = null;
	static InetSocketAddress CMtoME = null;
	static InetSocketAddress TMtoME = null;
	static InetSocketAddress CMtoGUI= null;
	//static InetSocketAddress MEtoTM = null;
	static String strReceiverName = null;
	static OSCClient senderTm;
	static OSCClient senderMe;
	static OSCServer receiverTm;
	static OSCServer receiverMe;
	static OSCServer receiverGui;
	static String clientName=null ;
	OSCMessage clientMsg;
	OSCMessage serverMsg = null;
	static JFrame frame = new JFrame("MotionComposer");
	static JTextArea textField = new JTextArea();
	//static SimpleOSCListener listener = new SimpleOSCListener();
	static String cmPath = Paths.get(".").toAbsolutePath().normalize().toString();
	
	
	public static void osc() throws IOException, InterruptedException 
	{
		
    	
    	   try {
	            // FileReader reads text files in the default encoding.
    		   	InputStream fileReader = new FileInputStream(strFileName);
    		    //BufferedReader buffReader = new BufferedReader((new InputStreamReader(fileReader)));
    		    
    		  // InputStream initialStream = new ByteArrayInputStream("With Java".getBytes());
    		     
    		    //Reader targetReader = new InputStreamReader(initialStream);
    		   	
	            //FileReader fileReaders = new FileReader(strFileName);

	            // Always wrap FileReader in BufferedReader.
	            BufferedReader bufferedReader = new BufferedReader((new InputStreamReader(fileReader)));

	            while((strLine = bufferedReader.readLine()) != null) {
	            	if(strLine.startsWith("@")) {
	            		strLine = strLine.substring(1);
	            	 	//System.out.println(line);	
	            		continue;
	               	}
	            	if(strLine.startsWith("#")) {
	            	    continue;	
	            	}
	            	if(strLine.contentEquals("CLIENT_SERVER_LIST")) 
	            	{
	            		strProcess= "CLIENT_SERVER_LIST";
	            		//System.out.println(process);
	            		continue;
	            	}
	                else if(strLine.contentEquals("OSC_MESSAGES_LIST"))
	                {
	                	strProcess= "OSC_MESSAGES_LIST";
	                	//System.out.println(process);
	                	continue;
						}
	            	
	            	if(strProcess== "CLIENT_SERVER_LIST") {
	            		
	            		csList.add(strLine);
	            		
									
	            	}
	            	else if (strProcess=="OSC_MESSAGES_LIST") 
	            	{
	            		oscMessageList.add(strLine);
	     
	            	}
	            	
	                }
	       
	            // Always close files.
	            bufferedReader.close(); 
	            //System.out.println(csList);
	        }
	        catch(FileNotFoundException ex) {
	            System.out.println(
	                "Unable to open file '" + 
	                strFileName + "'");                
	        }
	        catch(IOException ex) {
	            System.out.println(
	                "Error reading file '" 
	                + strFileName + "'");                  
	            
	        }
    	   
    	   for(String i : csList) 
	{
    		   HashMap<String, String> csMap = new HashMap<String,String>(); 
	        	strOldValue=i;
	        	strNewValue=strOldValue.replaceAll("'", "");
	    		strNewValue=strNewValue.substring(1,strNewValue.length()-1);
	    		strKeyValuePairs=strNewValue.split(",");

	    		for(String pair : strKeyValuePairs)                        
	            {
	                String[] entry = pair.split(":");            
	                csMap.put(entry[0].trim(), entry[1].trim());          

	            }
	    		
	    		String typeValue = csMap.get("type");
	    
	    		if(typeValue.contentEquals("client")) 
	    		{
	    			
	    			String strSenderIp= csMap.get("ip");
	    			senderIntAdd = InetAddress.getByName(strSenderIp);
	    			String strSenderPort = csMap.get("port");
	    			intSenderPort = Integer.parseInt(strSenderPort);
	    			clientName=csMap.get("name");
	    			senderTm = OSCClient.newUsing(OSCClient.UDP);
	    			
	    			switch(clientName) 
	    			{
	    				
	    				case "CMtoTM":
		    				CMtoTM  = new InetSocketAddress(senderIntAdd, intSenderPort);
		    				break;
	    				
	    				case "CMtoME":
		    				CMtoME = new InetSocketAddress(senderIntAdd, intSenderPort);
		    				break;
	    				
	    				case "CMtoGUI":
		    				CMtoGUI = new InetSocketAddress(senderIntAdd, intSenderPort);
		    				break;
	    				
	    				default:
	    					TMtoME = new InetSocketAddress(senderIntAdd, intSenderPort);
	    			}
	    			 			
	    		}
	    		else if(typeValue.contentEquals("server"))
	    		{
	    			String strReceiverIp= csMap.get("ip");
	    			receiverIntAdd = InetAddress.getByName(strReceiverIp);
	    			String strReceiverPort = csMap.get("port");
	    			inReceiverPort = Integer.parseInt(strReceiverPort);
	    			strReceiverName = csMap.get("name");
	    			//System.out.println(strReceiverName);
	    			switch(strReceiverName) 
	    			{
	    				case "CMserverTM":
	    					receiverTm = OSCServer.newUsing(OSCServer.UDP, new InetSocketAddress( receiverIntAdd, inReceiverPort),strReceiverName); // Create UDP Servers
		    				receiverTm.start();
		    				consoleMsg(strReceiverName);
		    				break;
		    				
	    				case "CMserverGUI":
	    					receiverGui = OSCServer.newUsing(OSCServer.UDP, new InetSocketAddress( receiverIntAdd, inReceiverPort),strReceiverName);
	    					receiverGui.start();
	    					break;
	    					
	    				default:
	    					receiverMe = OSCServer.newUsing(OSCServer.UDP, new InetSocketAddress( receiverIntAdd, inReceiverPort),strReceiverName); // Create UDP Servers
		    				receiverMe.start();
	    					
	    			}    			
	    		}
	    		
	     	}
    	   
    	 //
    	   
    	 receiverTm.addOSCListener(new OSCListener() 
    	   {
    	        public void messageReceived( OSCMessage m, SocketAddress addr, long time )
    	        	{
    	        		String receivedAddress = m.getName();
    	        		//String strNa =m.getName();
    	        		//consoleMsg(strNa);
    	        		String strAd = addr.toString();
    	        		consoleMsg(strAd);
    	        		
    	        		try {
							msghandler(receivedAddress);
						} 
    	        		catch (IOException e)
    	        		{
    	        			e.printStackTrace();
						} 
    	        		catch (InterruptedException e) 
    	        		{
							e.printStackTrace();
						}
    	        	}
    	   });
    	 
    	   
    	 receiverGui.addOSCListener(new OSCListener() 
    	 {
    		 public void messageReceived( OSCMessage m, SocketAddress addr, long time )
	        	{
    			 String receivedAddress = m.getName();
 	        		//consoleMsg(receivedAddress);
	        		String strrAd= addr.toString();
	        		//consoleMsg(strrAd);
	        		
	        		try {
						msghandler(receivedAddress);
					} 
	        		catch (IOException e)
	        		{
	        			e.printStackTrace();
					} 
	        		catch (InterruptedException e) 
	        		{
						e.printStackTrace();
					}
    	 }
    	 });
    	 
    	 
    	   receiverMe.addOSCListener(new OSCListener() 
    	   {
    	        public void messageReceived( OSCMessage m, SocketAddress addr, long time )
    	        	{
    	        	String receivedAddress = m.getName();
    	        	//consoleMsg(receivedAddress);
	        		String strrAd= addr.toString();
	        		//consoleMsg(strrAd);
	        		
	        		try {
						msghandler(receivedAddress);
					} 
	        		catch (IOException e)
	        		{
	        			e.printStackTrace();
					} 
	        		catch (InterruptedException e) 
	        		{
						e.printStackTrace();
					}
    	        	
    	        	}
    	   });
 	   	
	}

	public static void run_osc() throws IOException, InterruptedException 
	{	
				 
		 File fileMe = new File( cmPath+"\\TM\\release\\bin\\TrackingModule.exe");
	 	  Desktop desktopMe = Desktop.getDesktop();
	 	  if(fileMe.exists()) desktopMe.open(fileMe);
			   	   		 		  
	}
	
	public static void startME() throws IOException, InterruptedException
	{
			 	  File fileMe = new File( cmPath+"\\MEs\\Tonality\\180302_Tonality_test\\ME_tester.pd");
			 	  Desktop desktopMe = Desktop.getDesktop();
			 	  if(fileMe.exists()) desktopMe.open(fileMe);
	}
	

	public static void msghandler(String address) throws IOException, InterruptedException 
	{
		//System.out.println(address);
		
		if(address.equals("/set/loaded"))	
		
		{
				 senderTm.setTarget(CMtoME);
				 senderTm.start();
				 senderTm.send(new OSCMessage( "/set/initialize"));
				 consoleMsg("Msg Received:"+address);
				 String meInt="/set/initialize";
				 consoleMsg("Msg Sent:"+meInt+"\n");
				 senderTm.setTarget(CMtoGUI);
				 senderTm.start();
			     senderTm.send( new OSCMessage( "/set/loaded"));
		}                                     
		
		else if(address.equals("start")) 
		{
			//consoleMsg(address+"\n");
			startME();
		}
		
		
		else if (address.equals("/set/ME/ready")) 
		
		{
			 consoleMsg("Msg Received:"+address);
			 senderTm.setTarget(CMtoTM);
			 senderTm.start();
			 senderTm.send(new OSCMessage( "/set/player/0/tracking ,i 1"));
			 consoleMsg("Msg Sent:"+"/set/player/0/tracking ,i 1 to TM");
		}
		
		else if  ((address.equals("/set/player/1/soundbank/1/list"))|| (address.equals("/set/player/1/soundbank/2/list"))||
				  (address.equals("/set/player/2/soundbank/1/list"))||(address.equals("/set/player/2/soundbank/2/list"))) 
		
		{
	
				consoleMsg(address+"\n");
				
		
		}
		
		else if(address.equals("/set/error"))
		{
			
		}
		
		else if (address.equals("/set/zone/1/soundbank/1/list")||address.equals("/set/zone/2/soundbank/1/list")) 
		{
				consoleMsg(address+"\n");
		}
		
		else if (address =="/set/xxx") 
		
		{
			
		}
		
		else if (address == "/set/xxx") 
		
		{
		
		}

		else if(address.equals("/set/TM/loaded")) 
		
		{
				 //senderTm.setTarget(CMtoTM);
				 //senderTm.start();
				 //senderTm.send( new OSCMessage( "/set/TM/initialize"));
				 //String strTm = "/set/TM/initialize";
				 consoleMsg("Msg Received:"+address);
				 //consoleMsg("Msg Sent:"+strTm+"\n");
			 //System.out.println("msg sent");
		}
	
		else if(address.equals("/set/TM/ready")) 
		
		{	
				Socket tcpClient = new Socket("localhost", 50000);
				String nwMsg = "msg received";
				consoleMsg("Msg Received:"+address);
				consoleMsg(nwMsg+"\n");
				senderTm.setTarget(CMtoGUI);
				senderTm.start();
			    senderTm.send( new OSCMessage( "TM is Ready"));
			    //senderTm.setTarget(CMtoTM);
			    // senderTm.start();
			    //senderTm.send(new OSCMessage("/set/player/0/tracking ,i 1"));
		}
		
		else if(address.equals("/set/player/0/tracking ,i 0"))
		{
			consoleMsg("Msg Received:"+address);
		}
		else 
		{
			consoleMsg("Msg Received:"+address);
		}
		

	}
	
	public static void display() 
			
		{
		
			ImageIcon img = new ImageIcon("CM/includes/logo1.gif");
			frame.setSize(500,200);
		    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		    frame.setIconImage(img.getImage());
	
		    textField.setBounds(150, 150, 280, 150);
		    textField.setVisible(true);
		    frame.add(textField);
		    frame.setVisible(true);
		        
		    PipedOutputStream pOut = new PipedOutputStream();
		    System.setOut(new PrintStream(pOut,true));
      
		}
	
	public static void consoleMsg(String msgCon) 
	
		{
		
			textField.append(msgCon+"\n");
		
		}
	
	
	public static void send_osc()
		
		{
		
		
		//senderTm.send(clientMsg);
		}
	
}
