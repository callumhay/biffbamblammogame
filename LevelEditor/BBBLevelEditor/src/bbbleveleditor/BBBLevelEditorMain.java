package bbbleveleditor;

import java.awt.*;
import javax.swing.*;
import java.io.*;

public class BBBLevelEditorMain { 
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		BBBLevelEditMainWindow bbbLevelEditWin = new BBBLevelEditMainWindow("Biff! Bam!! Blammo!?! Level Editor");

		// Display the window
		bbbLevelEditWin.pack();
		bbbLevelEditWin.setVisible(true);
	}
}
