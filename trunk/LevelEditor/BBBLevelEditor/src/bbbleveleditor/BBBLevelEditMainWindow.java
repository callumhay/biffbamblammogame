package bbbleveleditor;

import java.awt.BorderLayout;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Toolkit;
import java.beans.PropertyVetoException;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

import javax.swing.DefaultListModel;
import javax.swing.ImageIcon;
import javax.swing.JDesktopPane;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JToolBar;
import javax.swing.ListSelectionModel;


public class BBBLevelEditMainWindow extends JFrame {

	private BBBLevelEditorMenuBar menuBar;
	private BBBLevelEditorToolBar toolBar;
	private JPanel topMostPanel;
	private JPanel levelPieceListParentPanel;
	private JList levelPieceList;
	private JDesktopPane levelEditDesktop;
	
	enum ToolMode { PAINT, ERASE };
	private ToolMode currToolMode;
	
	public BBBLevelEditMainWindow(String title) {
		super(title);
		JFrame.setDefaultLookAndFeelDecorated(true);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// Setup the parent panel that holds everything
		this.topMostPanel = new JPanel(new BorderLayout());
		this.topMostPanel.setPreferredSize(new Dimension(800, 600));
		
		// Setup the level edit desktop display
		this.levelEditDesktop = new JDesktopPane();
		this.topMostPanel.add(this.levelEditDesktop, BorderLayout.CENTER);		
		
		// Setup the menu bar and add the menu to the frame
		this.menuBar = new BBBLevelEditorMenuBar(this);
		this.setJMenuBar(menuBar);
		
		// Setup the toolbar and add the toolbar to the frame
		this.toolBar = new BBBLevelEditorToolBar(this);
		this.add(this.toolBar, BorderLayout.PAGE_START);
		
		// Setup the level piece list panel
		this.setupLevelPiecePanel();
		
		// Add the parent panel to the frame
		this.add(this.topMostPanel);
	}
	
	public void setToolMode(ToolMode mode) {
		this.currToolMode = mode;
	}
	public ToolMode getToolMode() {
		return this.currToolMode;
	}
	public LevelPiece getSelectedLevelPiece() {
		return (LevelPiece)this.levelPieceList.getSelectedValue();
	}
	
	/**
	 * Setup the level piece display panel (has a title and a list of level pieces).
	 */
	private void setupLevelPiecePanel() {
		// Parent panel for the entire block list area (hold info about the block selected and
		// the list of all possible blocks for the level)
		this.levelPieceListParentPanel = new JPanel(new BorderLayout());
		
		// Place the list in a scroll pane and insert it into the parent panel
		DefaultListModel tempLevelPieceList = this.setupLevelPieceList();
		this.levelPieceList = new JList(tempLevelPieceList);
		this.levelPieceList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		this.levelPieceList.setCellRenderer(new LevelPieceListRenderer());
		this.levelPieceList.setEnabled(true);
		this.levelPieceList.setToolTipText("List of possible level pieces for composing a level.");
		this.levelPieceList.setSelectedIndex(0);
		
		JScrollPane levelPieceListScrollPane = new JScrollPane(this.levelPieceList);
		JLabel levelPieceListTitle = new JLabel("Level Pieces");
		this.levelPieceListParentPanel.add(levelPieceListTitle, BorderLayout.NORTH);
		this.levelPieceListParentPanel.add(levelPieceListScrollPane, BorderLayout.CENTER);
		
		this.topMostPanel.add(this.levelPieceListParentPanel, BorderLayout.EAST);
	}
	
	/**
	 * Read all the level piece definitions into a default list and return it.
	 * @return A list will all the level piece types in it.
	 */
	private DefaultListModel setupLevelPieceList() {	
		DefaultListModel levelPieceList = new DefaultListModel();
		LevelPiece.LevelPieceCache.clear();
		
		try {
			File blockListFile = new File(BBBLevelEditorMain.class.getResource("resources/bbb_block_types.txt").toURI());
			if (blockListFile.canRead()) {
				
				BufferedReader blockListFileIn = new BufferedReader(new FileReader(blockListFile));
				String pieceName = "";
				String pieceImgFilename = "";
				String pieceSymbol = "";
				
				try {
					boolean firstValue = true;
					while (true) {
						String currLine = blockListFileIn.readLine();
						if (currLine == null) {
							break;
						}
						if (!currLine.isEmpty()) {
							// Read the expected format:
							// 1st line will be the name of the block type
							pieceName = currLine;
							
							// 2nd line will be the file path to the image of the block
							pieceImgFilename = blockListFileIn.readLine();
							// 3rd line will be the encoded value for that block in the BBB level file format
							pieceSymbol = blockListFileIn.readLine();
							
							LevelPiece newLevelPiece = new LevelPiece(pieceName, pieceSymbol, pieceImgFilename);
							levelPieceList.addElement(newLevelPiece);
							LevelPiece.LevelPieceCache.put(pieceName, newLevelPiece);
							
							// The very first piece read from file is set as the default piece (should be 'empty')
							if (firstValue) {
								LevelPiece.DefaultPiece = newLevelPiece;
								firstValue = false;
							}
							
							pieceImgFilename = "";
						}
					}
				}
				catch (Exception e) { 
					levelPieceList.removeAllElements();
					LevelPiece.LevelPieceCache.clear();
					JOptionPane.showMessageDialog(this, 
							"Invalid format or image file found in the level piece definitions file (resources/bbb_block_types.txt)!\n" +
							"Image name: " + pieceImgFilename, 
							"Error", JOptionPane.ERROR_MESSAGE);
					
				}
				finally { 
					blockListFileIn.close(); 
				}
			}
			else {
				JOptionPane.showMessageDialog(this, 
						"Could not read the level piece definitions file (resources/bbb_block_types.txt)!", 
						"Error", JOptionPane.ERROR_MESSAGE);
			}
		}
		catch (Exception e) {
			JOptionPane.showMessageDialog(this, 
					"Could not open the level piece definitions file (resources/bbb_block_types.txt)!", 
					"Error", JOptionPane.ERROR_MESSAGE);
		}
		
		return levelPieceList;
	}
	
	/**
	 * Add a new desktop internal frame that houses a level for editing. This should
	 * be called when the user requests a new level to edit.
	 * @param fileName The initial name of the new level.
	 * @param width The initial width of the new level.
	 * @param height The initial height of the new level.
	 */
	public void addNewLevelEditDocument(String fileName, int width, int height) {
		BBBLevelEditDocumentWindow newEditWindow = new BBBLevelEditDocumentWindow(this, fileName, width, height);
		newEditWindow.setVisible(true);
		this.levelEditDesktop.add(newEditWindow);
		
		try {
			newEditWindow.setSelected(true);
			newEditWindow.setMaximum(true);
		} catch (PropertyVetoException e) {
			e.printStackTrace();
		}
		
	}
	
	
}
