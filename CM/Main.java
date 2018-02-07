package moco;

import java.io.IOException;
import controlmodule.motioncomposer.OSC_CS;

public class Main {
	 
	public static void main(String[] args) throws IOException, InterruptedException {
		
			OSC_CS.display();
			OSC_CS.osc();
			OSC_CS.run_osc();
			 
		 
		  
		while(true) 
		{
			Thread.sleep(1);
		}
	}
	
	

}
