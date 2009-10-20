package bbbleveleditor;

import java.awt.*;
import javax.swing.*;
import java.io.*;

public class BBBLevelEditorMain {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		
		JFrame bbbLevelEditWin = new JFrame("Biff! Bam!! Blammo!?! Level Editor");
		JFrame.setDefaultLookAndFeelDecorated(true);
		bbbLevelEditWin.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// Setup the parent panel that holds everything
		JPanel topMostPanel = new JPanel(new BorderLayout());
		topMostPanel.setPreferredSize(new Dimension(800, 600));
		
		// Setup the menu bar *************************
		JMenuBar menuBar = new JMenuBar();
		
		// File menu...
		JMenu fileMenu = new JMenu("File");
		
		JMenuItem newMenuItem    = new JMenuItem("New");
		JMenuItem openMenuItem   = new JMenuItem("Open");
		JMenuItem closeMenuItem  = new JMenuItem("Close");
		JMenuItem saveMenuItem   = new JMenuItem("Save");
		JMenuItem saveAsMenuItem = new JMenuItem("Save As...");
		JMenuItem exitMenuItem   = new JMenuItem("Exit");
		
		fileMenu.add(newMenuItem);
		fileMenu.add(openMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(closeMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(saveMenuItem);
		fileMenu.add(saveAsMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(exitMenuItem);
		
		menuBar.add(fileMenu);
		
		// Edit menu...
		JMenu editMenu = new JMenu("Edit");
		
		JMenuItem lvlDimItem   = new JMenuItem("Dimensions...");
		JMenuItem lvlThemeItem = new JMenuItem("Theme...");
		JMenuItem lvlItemsItem = new JMenuItem("Allowable Item Drops...");
		
		editMenu.add(lvlDimItem);
		editMenu.add(lvlThemeItem);
		editMenu.add(lvlItemsItem);
		
		menuBar.add(editMenu);
		
		// Add the menu to the frame
		bbbLevelEditWin.setJMenuBar(menuBar);
		// ********************************************
		
		// Setup the toolbar **************************
		JToolBar toolBar = new JToolBar();
		
		ImageIcon selectionIcon  = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/selection_icon_16x16.png"));
		JButton selectionBtn  = new JButton(selectionIcon);
		ImageIcon paintBrushIcon = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/paintbrush_icon_16x16.png"));
		JButton paintBrushBtn = new JButton(paintBrushIcon);
		ImageIcon eraserIcon     = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/eraser_icon_16x16.png"));
		JButton eraserBtn     = new JButton(eraserIcon);
		
		toolBar.add(selectionBtn);
		toolBar.add(paintBrushBtn);
		toolBar.add(eraserBtn);
		
		// Add the toolbar to the frame
		topMostPanel.add(toolBar, BorderLayout.NORTH);
		// ********************************************
		
		// Setup the block list panel ****************************
		
		// Parent panel for the entire block list area (hold info about the block selected and
		// the list of all possible blocks for the level)
		JPanel blockListParentPanel = new JPanel(new BorderLayout());
		
		// Block information panel (shows block selected block and its information)
		JPanel selBlockInfoPanel = new JPanel(/* Layout? */);
		
		DefaultListModel blockList = new DefaultListModel();
		
		boolean blockListReadSuccess = false;
		try {
			File blockListFile = new File(BBBLevelEditorMain.class.getResource("resources/bbb_block_types.txt").toURI());
			if (blockListFile.canRead()) {
				
				BufferedReader blockListFileIn = new BufferedReader(new FileReader(blockListFile));
				blockListReadSuccess = true;
				
				try {
					while (true) {
						String currLine = blockListFileIn.readLine();
						if (!currLine.isEmpty()) {
							// Read the expected format:
							// 1st line will be the name of the block type
							String blockName = currLine;
							// 2nd line will be the file path to the image of the block
							String blockImgFilename = blockListFileIn.readLine();
							// 3rd line will be the encoded value for that block in the BBB level file format
							String blockSymbol = blockListFileIn.readLine();
							
							
							
							
						}
						// TODO: Read the format of the block file...
					}
				}
				catch (Exception e) { }
				finally { blockListFileIn.close(); }
			}
		}
		catch (Exception e) {}
		
		if (!blockListReadSuccess) {
			blockList.addElement("Block Type Definitions File not found.");
		}
		
		// TODO: Add a list of blocks
		
		// Place the list in a scroll pane and insert it into the parent panel
		JScrollPane blockListScrollPane = new JScrollPane(new JList(blockList));
		blockListParentPanel.add(selBlockInfoPanel, BorderLayout.NORTH);
		blockListParentPanel.add(blockListScrollPane, BorderLayout.CENTER);
		
		topMostPanel.add(blockListParentPanel, BorderLayout.EAST);
		// *******************************************************
		
		// Setup the edit level display ******************
		
		
		// ***********************************************
		
		
		// Add the parent panel to the frame
		bbbLevelEditWin.add(topMostPanel);
		
		// Display the window
		bbbLevelEditWin.pack();
		bbbLevelEditWin.setVisible(true);
		
	}

	//private static void SetupLevelPieceList() {	
	//}
	
}
