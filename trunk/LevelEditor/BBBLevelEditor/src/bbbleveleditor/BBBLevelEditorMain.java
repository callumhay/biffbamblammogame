package bbbleveleditor;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

public class BBBLevelEditorMain { 
	
	public static void main(String[] args) {
		try {
			try {
				UIManager.setLookAndFeel( UIManager.getSystemLookAndFeelClassName());
			} catch (ClassNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (InstantiationException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (UnsupportedLookAndFeelException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		
			BBBLevelEditMainWindow bbbLevelEditWin = new BBBLevelEditMainWindow("Biff! Bam!! Blammo!?! Level Editor");
	
			// Display the window
			bbbLevelEditWin.pack();
			bbbLevelEditWin.setVisible(true);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
}
