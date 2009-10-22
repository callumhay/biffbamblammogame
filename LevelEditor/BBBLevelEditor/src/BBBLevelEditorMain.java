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
		
		// Setup the menu bar and add the menu to the frame
		JMenuBar menuBar = new BBBLevelEditorMenuBar();
		bbbLevelEditWin.setJMenuBar(menuBar);
		
		// Setup the toolbar and add the toolbar to the frame
		JToolBar toolBar = new BBBLevelEditorToolBar();
		topMostPanel.add(toolBar, BorderLayout.NORTH);
		
		// Setup the block list panel:
		// Parent panel for the entire block list area (hold info about the block selected and
		// the list of all possible blocks for the level)
		JPanel levelPieceListParentPanel = new JPanel(new BorderLayout());

		// Place the list in a scroll pane and insert it into the parent panel
		DefaultListModel levelPieceList = SetupLevelPieceList(bbbLevelEditWin);
		JList levelPieceListComponent = new JList(levelPieceList);
		levelPieceListComponent.setCellRenderer(new LevelPieceListRenderer());
		levelPieceListComponent.setEnabled(true);
		levelPieceListComponent.setToolTipText("List of possible level pieces for composing a level.");
		
		JScrollPane levelPieceListScrollPane = new JScrollPane(levelPieceListComponent);
		JLabel levelPieceListTitle = new JLabel("Level Pieces");
		levelPieceListParentPanel.add(levelPieceListTitle, BorderLayout.NORTH);
		levelPieceListParentPanel.add(levelPieceListScrollPane, BorderLayout.CENTER);
		
		topMostPanel.add(levelPieceListParentPanel, BorderLayout.EAST);
		
		// Setup the edit level display
		JDesktopPane levelEditDesktop = new JDesktopPane();
		topMostPanel.add(levelEditDesktop, BorderLayout.CENTER);
		
		// Add the parent panel to the frame
		bbbLevelEditWin.add(topMostPanel);
		
		// Display the window
		bbbLevelEditWin.pack();
		bbbLevelEditWin.setVisible(true);
		
	}

	/**
	 * Read all the level piece definitions into a default list and return it.
	 * @return A list will all the level piece types in it.
	 */
	public static DefaultListModel SetupLevelPieceList(JFrame mainFrame) {	
		DefaultListModel levelPieceList = new DefaultListModel();

		try {
			File blockListFile = new File(BBBLevelEditorMain.class.getResource("resources/bbb_block_types.txt").toURI());
			if (blockListFile.canRead()) {
				
				BufferedReader blockListFileIn = new BufferedReader(new FileReader(blockListFile));
				String blockName = "";
				String blockImgFilename = "";
				String blockSymbol = "";
				
				try {
					while (true) {
						String currLine = blockListFileIn.readLine();
						if (currLine == null) {
							break;
						}
						if (!currLine.isEmpty()) {
							// Read the expected format:
							// 1st line will be the name of the block type
							blockName = currLine;
							// 2nd line will be the file path to the image of the block
							blockImgFilename = blockListFileIn.readLine();
							// 3rd line will be the encoded value for that block in the BBB level file format
							blockSymbol = blockListFileIn.readLine();
							
							levelPieceList.addElement(new LevelPiece(blockName, blockSymbol, blockImgFilename));
							blockImgFilename = "";
						}
						// TODO: Read the format of the block file...
					}
				}
				catch (Exception e) { 
					levelPieceList.removeAllElements();
					JOptionPane.showMessageDialog(mainFrame, 
							"Invalid format or image file found in the level piece definitions file (resources/bbb_block_types.txt)!\n" +
							"Image name: " + blockImgFilename, 
							"Error", JOptionPane.ERROR_MESSAGE);
				}
				finally { 
					blockListFileIn.close(); 
				}
			}
			else {
				JOptionPane.showMessageDialog(mainFrame, 
						"Could not read the level piece definitions file (resources/bbb_block_types.txt)!", 
						"Error", JOptionPane.ERROR_MESSAGE);
			}
		}
		catch (Exception e) {
			JOptionPane.showMessageDialog(mainFrame, 
					"Could not open the level piece definitions file (resources/bbb_block_types.txt)!", 
					"Error", JOptionPane.ERROR_MESSAGE);
		}
		
		return levelPieceList;
	}
	
}
