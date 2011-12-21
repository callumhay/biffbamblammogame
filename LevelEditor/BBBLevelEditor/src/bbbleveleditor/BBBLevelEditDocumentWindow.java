package bbbleveleditor;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.beans.PropertyVetoException;
import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Scanner;
import java.util.Set;
import java.util.TreeSet;
import java.util.prefs.Preferences;

import javax.swing.Box;
import javax.swing.ImageIcon;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JInternalFrame;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameListener;

import bbbleveleditor.BBBLevelEditMainWindow.ToolMode;

public class BBBLevelEditDocumentWindow extends JInternalFrame 
implements MouseMotionListener, MouseListener, InternalFrameListener {
	private static final long serialVersionUID = 1L;
	
	private BBBLevelEditMainWindow levelEditWindow = null;
	private String levelName = "";
	private String fileName = "";
	private int currWidth = -1;
	private int currHeight = -1;
	private File savedFileOnDisk = null;
	private boolean hasBeenModified = false;
	
	private boolean paintingPieces = false;
	private int[] starPointAmounts = new int[5];
	
	private JPanel levelDisplayPanel;           // panel where the level is displayed

	// access to the level piece names in this document [row (starting at the top)][col]
	private ArrayList<ArrayList<LevelPieceImageLabel>> pieces;	
	
	// A listing of all portal IDs present 
	private Set<Character> portalIDs = new TreeSet<Character>();
	private Set<Character> teslaIDs  = new TreeSet<Character>();
	
	// Settings for all the drop items in the level
	private HashMap<String, Integer> itemDropSettings;
	
	public BBBLevelEditDocumentWindow(BBBLevelEditMainWindow window) {
		super("", true, true, true, true);
		
		this.levelEditWindow = window;
		this.fileName = this.levelName = "";
		this.currWidth = 0;
		this.currHeight = 0;
		
		this.starPointAmounts[0] = 100;
		this.starPointAmounts[1] = 500;
		this.starPointAmounts[2] = 1000;
		this.starPointAmounts[3] = 2000;
		this.starPointAmounts[4] = 5000;
		
		this.pieces = null;
		this.itemDropSettings = ItemDrop.populateDefaultItemDropSettings();
		this.hasBeenModified = true;
	}	
	
	public BBBLevelEditDocumentWindow(BBBLevelEditMainWindow window, String fileName, int width, int height) {
		super(fileName + " (" + width + "x" + height + ")", true, true, true, true);
			
		this.levelEditWindow = window;
		this.fileName = this.levelName = fileName;
		this.currWidth = width;
		this.currHeight = height;
		this.itemDropSettings = ItemDrop.populateDefaultItemDropSettings();
		this.hasBeenModified = true;
		
		this.starPointAmounts[0] = 100;
		this.starPointAmounts[1] = 500;
		this.starPointAmounts[2] = 1000;
		this.starPointAmounts[3] = 2000;
		this.starPointAmounts[4] = 5000;
		
		this.setupUI();
		this.initDefaultPieces();
	}
	
	public int[] getStarPointAmounts() {
		return this.starPointAmounts;
	}
	public void setStarPointAmounts(int[] amounts) {
		this.starPointAmounts = amounts;
	}
	
	public String getLevelName() {
		return this.levelName;
	}
	public void setLevelName(String name) {
		this.levelName = name;
	}
	
	private void setupUI() {
		// Create the level display panel with its grid of level pieces...
		this.levelDisplayPanel = new JPanel(new GridLayout(this.currHeight, this.currWidth));
		this.levelDisplayPanel.setPreferredSize(new Dimension(this.currWidth*LevelPiece.LEVEL_PIECE_WIDTH, 
				this.currHeight*LevelPiece.LEVEL_PIECE_HEIGHT));
		this.levelDisplayPanel.setMinimumSize(new Dimension(this.currWidth*LevelPiece.LEVEL_PIECE_WIDTH, 
				this.currHeight*LevelPiece.LEVEL_PIECE_HEIGHT));
		this.levelDisplayPanel.setMaximumSize(new Dimension(this.currWidth*LevelPiece.LEVEL_PIECE_WIDTH, 
				this.currHeight*LevelPiece.LEVEL_PIECE_HEIGHT));	
		
		// Add listeners to the level display panel for the various tools to be able
		// to change the level
		this.levelDisplayPanel.addMouseMotionListener(this);
		this.levelDisplayPanel.addMouseListener(this);
		
		// Set the layout for the level - always centered at the exact size of the pieces
		Box tempVLayout = Box.createVerticalBox();
		tempVLayout.add(Box.createVerticalGlue());
		tempVLayout.add(this.levelDisplayPanel);
		tempVLayout.add(Box.createVerticalGlue());
		
		Box tempHLayout = Box.createHorizontalBox();
		tempHLayout.add(Box.createHorizontalGlue());
		tempHLayout.add(tempVLayout);
		tempHLayout.add(Box.createHorizontalGlue());
	
		JScrollPane levelDisplayScroll = new JScrollPane(tempHLayout);
		this.setContentPane(levelDisplayScroll);
		
		this.addInternalFrameListener(this);
	}
	
	private void initDefaultPieces() {
		// Set all the pieces to the default piece and set the graphics in the panel
		// for those pieces as well
		this.pieces = new ArrayList<ArrayList<LevelPieceImageLabel>>(this.currHeight);
		for (int i = 0; i < this.currHeight; i++) {
			ArrayList<LevelPieceImageLabel> currRow = new ArrayList<LevelPieceImageLabel>(this.currWidth);
			for (int j = 0; j < this.currWidth; j++) {
				
				LevelPieceImageLabel tempLabel = new LevelPieceImageLabel(LevelPiece.DefaultPiece);
				tempLabel.setPreferredSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
				tempLabel.setMinimumSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
				tempLabel.setMaximumSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
				
				currRow.add(tempLabel);
				this.levelDisplayPanel.add(tempLabel);
			}
			this.pieces.add(i, currRow);
		}
		this.pack();
	}
	
	public void setLevelDimensions(int newWidth, int newHeight) {
		if (newWidth <= 0 || newHeight <= 0) {
			assert(false);
			return;
		}	
		int windowHeight = this.getHeight();
		int windowWidth = this.getWidth();
		boolean wasMaximum = this.isMaximum();
		
		if (newWidth < this.currWidth) {
			// Shrink the width from the right side
			int widthDiff = this.currWidth - newWidth;
			
			for (int row = 0; row < this.currHeight; row++) {
				ArrayList<LevelPieceImageLabel> currRow = this.pieces.get(row);
				for (int i = 0; i < widthDiff; i++) {
					currRow.remove(currRow.size()-1);
				}
			}
			
			// Setup the pieces again...
			this.currWidth = newWidth;
			this.resetPieces();
		}
		else if (newWidth > this.currWidth) {
			// Grow the width from the right side
			int widthDiff = newWidth - this.currWidth;
			
			for (int row = 0; row < this.currHeight; row++) {
				ArrayList<LevelPieceImageLabel> currRow = this.pieces.get(row);
				for (int i = 0; i < widthDiff; i++) {
					LevelPieceImageLabel tempLabel = new LevelPieceImageLabel(LevelPiece.DefaultPiece);
					tempLabel.setPreferredSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
					tempLabel.setMinimumSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
					tempLabel.setMaximumSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
					currRow.add(tempLabel);
				}
			}
			
			// Setup the pieces again...
			this.currWidth = newWidth;
			this.resetPieces();
		}
		
		if (newHeight < this.currHeight) {
			// Shrink the height from the top
			int heightDiff = this.currHeight - newHeight;
			
			for (int i = 0; i < heightDiff; i++) {
				this.pieces.remove(0);
			}
		
			// Setup the pieces again...
			this.currHeight = newHeight;
			this.resetPieces();	
		}
		else if (newHeight > this.currHeight) {
			// Grow the height from the top
			int heightDiff = newHeight - this.currHeight;
		
			for (int i = 0; i < heightDiff; i++) {
				ArrayList<LevelPieceImageLabel> newRow = new ArrayList<LevelPieceImageLabel>(this.currWidth);
				for (int j = 0; j < this.currWidth; j++) {
					
					LevelPieceImageLabel tempLabel = new LevelPieceImageLabel(LevelPiece.DefaultPiece);
					tempLabel.setPreferredSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
					tempLabel.setMinimumSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
					tempLabel.setMaximumSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
					
					newRow.add(tempLabel);
				}
				this.pieces.add(0, newRow);
			}
			
			// Setup the pieces again...
			this.currHeight = newHeight;
			this.resetPieces();
		}
		
		this.pack();
		this.setSize(new Dimension(windowWidth, windowHeight));
		if (wasMaximum) {
			try {
				this.setMaximum(true);
			} 
			catch (PropertyVetoException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
	
	public void setItemDropSettings(HashMap<String, Integer> settings) {
		assert(settings.size() == this.itemDropSettings.size());
		this.itemDropSettings = settings;
		this.hasBeenModified = true;
	}
	public HashMap<String, Integer> getItemDropSettings() {
		return this.itemDropSettings;
	}
	
	public void setFileName(String fileName) {
		this.fileName = fileName;
		this.setTitle(this.fileName + " (" + this.currWidth + "x" + this.currHeight + ")");
	}
	public String getFileName() {
		return this.fileName;
	}
	public int GetLevelWidth() {
		return this.currWidth;
	}
	public int GetLevelHeight() {
		return this.currHeight;
	}

	public boolean open() {
		// Use preferences to store and retrieve the last open location
		Preferences prefs = Preferences.userRoot().node(this.getClass().getName());
		
		JFileChooser openDlg = new JFileChooser();
		openDlg.setFileFilter(new BBBLevelFileFilter());
		openDlg.setFileHidingEnabled(true);
		openDlg.setMultiSelectionEnabled(false);
		openDlg.setAcceptAllFileFilterUsed(false);
		
		String openDir = prefs.get("lastOpenLocation", "");
		if (!openDir.isEmpty()) {
			openDlg.setCurrentDirectory(new File(openDir));
		}
		
		int result = openDlg.showOpenDialog(this.levelEditWindow);
		if (result == JFileChooser.APPROVE_OPTION) {
			
			String openDirToSave = openDlg.getSelectedFile().getPath();
			prefs.put("lastOpenLocation", openDirToSave.substring(0, openDirToSave.lastIndexOf(File.separatorChar)+1));
			this.savedFileOnDisk = openDlg.getSelectedFile();
			if (this.savedFileOnDisk == null) {
				assert(false);
				return false;
			}
			
			Scanner levelFileScanner = null;
			try {
				levelFileScanner = new Scanner(this.savedFileOnDisk);
				boolean goodFormat = true;
				// Start by reading the name...
				goodFormat = levelFileScanner.hasNextLine();
				if (!goodFormat) {
					throw new Exception();
				}
				this.levelName = levelFileScanner.nextLine();
				
				// ...width and height of the level
				goodFormat = levelFileScanner.hasNextInt();
				if (!goodFormat) {
					throw new Exception();
				}
				this.currWidth = levelFileScanner.nextInt();
				if (this.currWidth < 0 || this.currWidth > 500) {
					JOptionPane.showMessageDialog(this, "Level width is too large, width must be positive and less than or equal to 500.", "Could not read", JOptionPane.ERROR_MESSAGE);
				}
				
				goodFormat = levelFileScanner.hasNextInt();
				if (!goodFormat) {
					throw new Exception();
				}
				this.currHeight = levelFileScanner.nextInt();
				if (this.currHeight < 0 || this.currHeight > 500) {
					JOptionPane.showMessageDialog(this, "Level height is too large, width must be positive and less than or equal to 500.", "Could not read", JOptionPane.ERROR_MESSAGE);
				}
				
				// Now read in all the pieces in the level
				//levelFileScanner.useDelimiter(java.util.regex.Pattern.compile("\\s"));
				String currLevelPieceSymbol = "";
				//LevelPiece currLevelPiece = null;
				
				this.setupUI();
				
				this.pieces = new ArrayList<ArrayList<LevelPieceImageLabel>>(this.currHeight);
				for (int row = 0; row < this.currHeight; row++) {
					ArrayList<LevelPieceImageLabel> currRow = new ArrayList<LevelPieceImageLabel>(this.currWidth);
					
					for (int col = 0; col < this.currWidth; col++) {
						
						currLevelPieceSymbol = levelFileScanner.next();
						LevelPieceImageLabel tempLabel = new LevelPieceImageLabel(currLevelPieceSymbol);

						if (tempLabel.getIsPortal()) {
							this.portalIDs.add(tempLabel.getBlockID());
							this.portalIDs.add(tempLabel.getPortalSiblingID());
						}
						else if (tempLabel.getIsTesla()) {
							this.teslaIDs.add(tempLabel.getBlockID());
							this.teslaIDs.addAll(tempLabel.getSiblingIDs());
						}
						
						currRow.add(tempLabel);
						this.levelDisplayPanel.add(tempLabel);
					}
					
					this.pieces.add(row, currRow);
				}
				this.pack();
				
				// Read the stars or the various item likelihoods - this can be variable for backwards compatibility
				if (levelFileScanner.hasNext("STARS:")) {
					this.readStarAmounts(levelFileScanner);
					this.readItemLiklihoods(levelFileScanner);
				}
				else {
					this.readItemLiklihoods(levelFileScanner);
					this.readStarAmounts(levelFileScanner);
				}
				
				this.fileName = this.savedFileOnDisk.getName();
				this.fileName = this.fileName.substring(0, this.fileName.lastIndexOf('.'));
				this.setTitle(this.fileName + " (" + this.currWidth + "x" + this.currHeight + ")");
			} 
			catch (FileNotFoundException e) {
				JOptionPane.showMessageDialog(this, "Failed to open file.", "Could not read", JOptionPane.ERROR_MESSAGE);
				this.savedFileOnDisk = null;
				this.fileName = "";
				this.currHeight = this.currWidth = 0;
				return false;
			}
			catch (Exception e) {
				JOptionPane.showMessageDialog(this, "File could not be read.", "Could not read", JOptionPane.ERROR_MESSAGE);
				this.savedFileOnDisk = null;
				this.fileName = "";
				this.currHeight = this.currWidth = 0;
				return false;
			}
			finally {
				if (levelFileScanner != null) {
					levelFileScanner.close();
				}
			}
		}
		else {
			return false;
		}
		
		this.hasBeenModified = false;
		return true;
	}
	
	private boolean readStarAmounts(Scanner levelFileScanner) {
		if (!levelFileScanner.hasNext("STARS:")) { return true; }
		// Read the "STARS:" header
		levelFileScanner.next();
		for (int i = 0; i < 5; i++) {
			this.starPointAmounts[i] = levelFileScanner.nextInt();
		}
		return true;
	}
	
	private boolean readItemLiklihoods(Scanner levelFileScanner) {
		while (levelFileScanner.hasNext()) {
			String possibleItemDropName = levelFileScanner.next();
			boolean itemExists = this.itemDropSettings.containsKey(possibleItemDropName);
			if (itemExists) {
				int likelihood = levelFileScanner.nextInt();
				this.itemDropSettings.put(possibleItemDropName, likelihood);
			}
		}
		
		return true;
	}
	
	public boolean save() {
		if (this.savedFileOnDisk == null) {
			return this.saveAs();
		}
		else if (!this.savedFileOnDisk.canWrite()) {
			JOptionPane.showMessageDialog(this, "Could not write to disk, please save to a writable file.", "Could not write", JOptionPane.ERROR_MESSAGE);
			return this.saveAs();
		}
		else {
			return this.saveToFile();
		}
	}
	
	public boolean saveAs() {
		String renameToProperExt = BBBLevelFileFilter.changeExtensionToBBBLvlExt(this.fileName);
		
		// Use preferences to store and retrieve the last save location
		Preferences prefs = Preferences.userRoot().node(this.getClass().getName());	
		
		JFileChooser saveAsDlg = new JFileChooser();
		saveAsDlg.setFileFilter(new BBBLevelFileFilter());
		saveAsDlg.setAcceptAllFileFilterUsed(false);
		
		String saveAsDir = prefs.get("lastSaveAsLocation", "");
		if (!saveAsDir.isEmpty()) {
			saveAsDlg.setCurrentDirectory(new File(saveAsDir));
		}
		
		String temp = saveAsDlg.getCurrentDirectory().getPath() + File.separatorChar + renameToProperExt;
		saveAsDlg.setSelectedFile(new File(temp));

		int result = saveAsDlg.showSaveDialog(this.levelEditWindow);
		
		if (result == JFileChooser.APPROVE_OPTION) {
			this.savedFileOnDisk = saveAsDlg.getSelectedFile();
			renameToProperExt = BBBLevelFileFilter.changeExtensionToBBBLvlExt(this.savedFileOnDisk.getPath());
			this.savedFileOnDisk.renameTo(new File(renameToProperExt));
			
			if (this.savedFileOnDisk.exists()) {
				int response = JOptionPane.showConfirmDialog (null,
			               "Overwrite existing file?","Confirm Overwrite",
			                JOptionPane.OK_CANCEL_OPTION,
			                JOptionPane.QUESTION_MESSAGE);
				if (response == JOptionPane.CANCEL_OPTION) {
					return false;
			    }
			}
			
			String tempPath = this.savedFileOnDisk.getPath();
			prefs.put("lastSaveAsLocation", tempPath.substring(0, tempPath.lastIndexOf(File.separatorChar)+1));	
			
			return this.saveToFile();
		}
		
		return false;
	}
	
	private boolean saveToFile() {
		Writer levelFileWriter = null;
		try {
			levelFileWriter = new FileWriter(this.savedFileOnDisk);
			
			// File starts with the name of the level and the width and height of the level...
			levelFileWriter.write(this.levelName + "\r\n");
			levelFileWriter.write(this.currWidth + " " + this.currHeight + "\r\n");
			
			// Followed by the level pieces...
			for (int row = 0; row < this.currHeight; row++) {
				for (int col = 0; col < this.currWidth; col++) {
					
					LevelPieceImageLabel currPieceLbl = this.pieces.get(row).get(col);
					LevelPiece currLvlPiece = currPieceLbl.getLevelPiece();
					assert(currLvlPiece != null);
					
					if (!currPieceLbl.getIsValid()) {
						JOptionPane.showMessageDialog(this, "Failed to write level file due to invalid block.", 
								"Level Format Error", JOptionPane.ERROR_MESSAGE);
						throw new Exception();
					}
					
					// If we're dealing with a portal block make sure its name and sibling are valid
					if (currPieceLbl.getIsPortal()) {
						if (!currPieceLbl.getHasIDsInGivenSet(this.portalIDs)) {
							JOptionPane.showMessageDialog(this, "Failed to write level file due to invalid portal ids.", 
									"Level Format Error", JOptionPane.ERROR_MESSAGE);
							throw new Exception();
						}
						
						levelFileWriter.write(currLvlPiece.getSymbol() + "(" + currPieceLbl.getBlockID() + 
								"," + currPieceLbl.getPortalSiblingID() + ")");
					}
					else if (currPieceLbl.getIsTesla()) {
						if (!currPieceLbl.getHasIDsInGivenSet(this.teslaIDs)) {
							JOptionPane.showMessageDialog(this, "Failed to write level file due to invalid tesla block ids.", 
									"Level Format Error", JOptionPane.ERROR_MESSAGE);
							throw new Exception();
						}
						String outputStr = currLvlPiece.getSymbol() + "(" + currPieceLbl.getIsTeslaOn() + "," + currPieceLbl.getIsTeslaChangable() + "," + currPieceLbl.getBlockID() + ",";
						Iterator<Character> iter = currPieceLbl.getSiblingIDs().iterator();
						while (iter.hasNext()) {
							outputStr = outputStr + iter.next();
							if (iter.hasNext()) {
								outputStr = outputStr + ",";
							}
						}
						outputStr = outputStr + ")";
						levelFileWriter.write(outputStr);
					}
					else if (currPieceLbl.getIsItemDrop()) {
						String outputStr = currLvlPiece.getSymbol() + "(";
						Iterator<String> iter = currPieceLbl.getItemDropTypes().iterator();
						while (iter.hasNext()) {
							outputStr = outputStr + iter.next();
							if (iter.hasNext()) {
								outputStr = outputStr + ",";
							}
						}
						outputStr = outputStr + ")";
						levelFileWriter.write(outputStr);
					}
					else if (currPieceLbl.getIsCannonBlock()) {
						
						String outputStr = currLvlPiece.getSymbol() + "(";
						if (currPieceLbl.getCannonBlockDegAngle1() == currPieceLbl.getCannonBlockDegAngle2()) {
							outputStr = outputStr + currPieceLbl.getCannonBlockDegAngle1();
						}
						else {
							outputStr = outputStr + currPieceLbl.getCannonBlockDegAngle1() + "-" + currPieceLbl.getCannonBlockDegAngle2();
						}
						outputStr = outputStr + ")";
						levelFileWriter.write(outputStr);
					}
					else if (currPieceLbl.getIsSwitchBlock()) {
						String outputStr = currLvlPiece.getSymbol() + "(" + currPieceLbl.getSwitchTriggerID() + ")";
						levelFileWriter.write(outputStr);
					}
					else {
						levelFileWriter.write(currLvlPiece.getSymbol());
					}
					
					// Write the trigger ID if there is one...
					if (currPieceLbl.getTriggerID() != LevelPiece.NO_TRIGGER_ID) {
						levelFileWriter.write("{" + currPieceLbl.getTriggerID() + "}");
					}
					
					levelFileWriter.write(" ");
				}
				levelFileWriter.write("\r\n");
			}
			levelFileWriter.write("\r\n");
			
			// Add the star point milestones
			levelFileWriter.write("STARS:\r\n");
			for (int i = 0; i < 5; i++) {
				levelFileWriter.write("" + this.starPointAmounts[i]);
				levelFileWriter.write("\r\n");
			}
			levelFileWriter.write("\r\n");
			
			// And the item drop settings...
			Iterator<String> itemDropIter = this.itemDropSettings.keySet().iterator();
			while (itemDropIter.hasNext()) {
				String itemDropName = itemDropIter.next();
				Integer itemDropLikelihood = this.itemDropSettings.get(itemDropName);
				levelFileWriter.write(itemDropName + " " + itemDropLikelihood + "\r\n");
			}
			
			this.fileName = this.savedFileOnDisk.getName();
			this.fileName = this.fileName.substring(0, this.fileName.lastIndexOf('.'));
			this.setTitle(this.fileName + " (" + this.currWidth + "x" + this.currHeight + ")");
		} 
		catch (Exception e) {
			JOptionPane.showMessageDialog(this, "Failed to write to disk.", "Could not write", JOptionPane.ERROR_MESSAGE);
			return false;
		}
		finally {
			if (levelFileWriter != null) {
				try {
					levelFileWriter.close();
				} 
				catch (IOException e) {
				}
			}
		}
		this.hasBeenModified = false;
		return true;
	}
	
	private void resetPieces() {
		this.levelDisplayPanel.removeAll();
		this.levelDisplayPanel.setLayout(new GridLayout(this.currHeight, this.currWidth));
		this.levelDisplayPanel.setPreferredSize(new Dimension(this.currWidth*LevelPiece.LEVEL_PIECE_WIDTH, 
				this.currHeight*LevelPiece.LEVEL_PIECE_HEIGHT));
		this.levelDisplayPanel.setMinimumSize(new Dimension(this.currWidth*LevelPiece.LEVEL_PIECE_WIDTH, 
				this.currHeight*LevelPiece.LEVEL_PIECE_HEIGHT));
		this.levelDisplayPanel.setMaximumSize(new Dimension(this.currWidth*LevelPiece.LEVEL_PIECE_WIDTH, 
				this.currHeight*LevelPiece.LEVEL_PIECE_HEIGHT));	
		
		// Followed by the level pieces...
		for (int row = 0; row < this.currHeight; row++) {
			for (int col = 0; col < this.currWidth; col++) {
				this.levelDisplayPanel.add(this.pieces.get(row).get(col));
			}
		}

		this.setTitle(this.fileName + " (" + this.currWidth + "x" + this.currHeight + ")");
		this.pack();
		
		this.hasBeenModified = true;
	}
	
	private void paintPieces(Point p) {
		BBBLevelEditMainWindow.ToolMode toolMode = this.levelEditWindow.getToolMode();
		if (toolMode == BBBLevelEditMainWindow.ToolMode.PAINT) {
			LevelPiece levelPieceToPaint = this.levelEditWindow.getSelectedLevelPiece();
			this.changeLevelPieceAtLocation(p, levelPieceToPaint);
		}
		else if (toolMode == BBBLevelEditMainWindow.ToolMode.ERASE) {
			this.changeLevelPieceAtLocation(p, LevelPiece.DefaultPiece);
		}		
	}
	
	@Override
	public void mouseDragged(MouseEvent e) {
		if (this.paintingPieces) {
			this.paintPieces(e.getPoint());
		}
	}

	@Override
	public void mouseMoved(MouseEvent e) {
	}
	
	private LevelPieceImageLabel getPieceAtLocation(Point loc) {
		// First of all make sure the location is within the bounds of the
		// level currently being edited
		int x = Math.min(loc.x, this.currWidth*(LevelPiece.LEVEL_PIECE_WIDTH));
		x = Math.max(x, 0);
		int y = Math.min(loc.y, this.currHeight*(LevelPiece.LEVEL_PIECE_HEIGHT));
		y = Math.max(y, 0);
		
		// Now figure out the index of the piece to lookup in the 2D arraylist of pieces
		float colIndexFlt = (float)x / (float)LevelPiece.LEVEL_PIECE_WIDTH;
		float rowIndexFlt = (float)y / (float)LevelPiece.LEVEL_PIECE_HEIGHT;
		
		int rowIndex = (int)Math.floor(rowIndexFlt);
		int colIndex = (int)Math.floor(colIndexFlt);
		
		rowIndex = Math.min(this.currHeight-1, Math.max(0, rowIndex));
		colIndex = Math.min(this.currWidth-1, Math.max(0, colIndex));
		
		ArrayList<LevelPieceImageLabel> levelPieceRow = this.pieces.get(rowIndex);
		return levelPieceRow.get(colIndex);
	}
	
	public void changeLevelPieceAtLocation(Point loc, LevelPiece newPiece) {
		LevelPieceImageLabel editPiece = this.getPieceAtLocation(loc);
		
		// If we're getting rid of a portal block then remove its ID from the list...
		if (editPiece.getIsPortal()) {
			this.portalIDs.remove(editPiece.getBlockID());
		}
		else if (editPiece.getIsTesla()) {
			this.teslaIDs.remove(editPiece.getTeslaID());
		}
		
		LevelPiece oldPiece = editPiece.getLevelPiece();
		editPiece.setLevelPiece(newPiece);
		this.hasBeenModified = true;
		
		// In the case of a portal block we ask the user for a valid, unique character ID
		boolean wasSet = true;
		if (editPiece.getIsPortal()) {
			wasSet = this.readIDAndSiblings(editPiece, oldPiece, false);
		}
		else if (editPiece.getIsTesla()) {
			wasSet = this.readIDAndSiblings(editPiece, oldPiece, true);
		}
		
		if (!wasSet) {
			// Operation was canceled, add the ID back...
			if (editPiece.getIsPortal()) {
				this.portalIDs.add(editPiece.getBlockID());
			}
			else if (editPiece.getIsTesla()) {
				this.teslaIDs.add(editPiece.getTeslaID());
			}	
		}
	}

	private boolean readIDAndSiblings(LevelPieceImageLabel editPiece, LevelPiece oldPiece, boolean allowMultipleSiblings) {
		// Start with getting just the block ID
		char blockID = ' ';
		String blockIDStr = "";
		while (blockIDStr.length() != 1) {
			blockIDStr = (String)JOptionPane.showInputDialog(
	                this.levelEditWindow,
	                "Enter a single, unique character name\r\nfor the block:",
	                "Block Name Entry",
	                JOptionPane.PLAIN_MESSAGE,
	                null, null, null);
			
			// The user cancelled the operation...
			if (blockIDStr == null) {
				editPiece.setLevelPiece(oldPiece);
				return false;
			}
			
			if (blockIDStr.length() != 1 || !LevelPieceImageLabel.IsValidBlockID(blockIDStr.charAt(0))) {
				JOptionPane.showMessageDialog(this.levelEditWindow, "Invalid value, must be a single, unique alpha numeric character!");
				blockIDStr = "";
			}
			else {
				blockID = blockIDStr.charAt(0);
				if (allowMultipleSiblings && this.teslaIDs.contains(blockID)) {
					JOptionPane.showMessageDialog(this.levelEditWindow, "Invalid value, must be a single, unique alpha numeric character!");
					blockIDStr = "";
				}
				else if (!allowMultipleSiblings && this.portalIDs.contains(blockID)) {
					JOptionPane.showMessageDialog(this.levelEditWindow, "Invalid value, must be a single, unique alpha numeric character!");
					blockIDStr = "";	
				}
			}
		}
		
		// Now get the sibling(s) of the block
		Set<Character> siblingIDs = null;
		while (siblingIDs == null) {
			if (allowMultipleSiblings) {
				String enteredSiblings = "";
				enteredSiblings = (String)JOptionPane.showInputDialog(
	                    this.levelEditWindow,
	                    "Enter one or more, unique character names\r\nfor the block's siblings, seperated by commas:",
	                    "Sibling Names Entry",
	                    JOptionPane.PLAIN_MESSAGE,
	                    null, null, null);
				
				String[] splitStr = enteredSiblings.split("\\,");
				siblingIDs = new TreeSet<Character>();
				for (int i = 0; i < splitStr.length; i++) {
					String currSibling = splitStr[i];
					currSibling.trim();
					if (currSibling.length() != 1 || !LevelPieceImageLabel.IsValidBlockID(currSibling.charAt(0))) {
						siblingIDs = null;
						JOptionPane.showMessageDialog(this.levelEditWindow, "Invalid value, each sibling must be a single, unique alpha numeric character!");
						continue;
					}

					boolean wasUnique = siblingIDs.add(currSibling.charAt(0));
					if (!wasUnique) {
						siblingIDs = null;
						JOptionPane.showMessageDialog(this.levelEditWindow, "Invalid value, each sibling must be a single, unique alpha numeric character!");
						continue;
					}
				}
			}
			else {
				// Check for the id among the other blocks already existing with siblings...
				for (int row = 0; row < this.currHeight; row++) {
					for (int col = 0; col < this.currWidth; col++) {
						
						LevelPieceImageLabel currPieceLbl = this.pieces.get(row).get(col);
						if (currPieceLbl.getIsPortal()) {
							if (currPieceLbl.getPortalSiblingID() == blockID) {
								siblingIDs = new TreeSet<Character>();
								siblingIDs.add(currPieceLbl.getBlockID());
								break;
							}
						}
					}
				}
				if (siblingIDs != null) {
					break;
				}
				
				String inputSiblingID = (String)JOptionPane.showInputDialog(
	                    this.levelEditWindow,
	                    "Enter a single, unique character name\r\nfor the block's sibling:",
	                    "Sibling Name Entry",
	                    JOptionPane.PLAIN_MESSAGE,
	                    null, null, null);
				
				// The user cancelled the operation...
				if (inputSiblingID == null) {
					editPiece.setLevelPiece(oldPiece);
					return false;
				}
				
				if (inputSiblingID.length() == 1 && !inputSiblingID.equals(blockID) && 
					LevelPieceImageLabel.IsValidBlockID(inputSiblingID.charAt(0))) {
					
					siblingIDs = new TreeSet<Character>();
					siblingIDs.add(inputSiblingID.charAt(0));
				}
				else {
					JOptionPane.showMessageDialog(this.levelEditWindow, "Invalid value, must be a single, unique alpha numeric character!");
				}
			}
		}
		
		if (allowMultipleSiblings) {
			assert(siblingIDs.size() >= 1);
			editPiece.setBlockID(blockID);
			editPiece.setSiblingIDs(siblingIDs);
			this.teslaIDs.add(blockID);
		}
		else {
			assert(siblingIDs.size() == 1);
			editPiece.setBlockID(blockID);
			editPiece.setPortalSiblingID(siblingIDs.iterator().next());
			this.portalIDs.add(blockID);
		}
		
		return true;
	}
	
	@Override
	public void mouseClicked(MouseEvent e) {
		this.maybeShowContextMenu(e);
	}

	@Override
	public void mouseEntered(MouseEvent arg0) {
		BBBLevelEditMainWindow.ToolMode toolMode = this.levelEditWindow.getToolMode();
		Toolkit toolkit = Toolkit.getDefaultToolkit();
		Cursor cursor = null;
		if (toolMode == ToolMode.PAINT) {
			ImageIcon paintBrushIcon = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/paintbrush_icon_32x32.png"));
			cursor = toolkit.createCustomCursor(paintBrushIcon.getImage(), new Point(16, 16), "Paint Cursor");
		}
		else if (toolMode == ToolMode.ERASE) {
			ImageIcon eraserIcon = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/eraser_icon_32x32.png"));
			cursor = toolkit.createCustomCursor(eraserIcon.getImage(), new Point(16, 16), "Erase Cursor");
		}
		else {
			assert false;
			return;
		}
		
		this.levelDisplayPanel.setCursor(cursor);
	}

	@Override
	public void mouseExited(MouseEvent arg0) {	
	}

	private class ContextMenuListener implements ActionListener {
		private JDialog dlg = null;

		ContextMenuListener(JDialog dlg) {
			this.dlg = dlg;
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			this.dlg.setVisible(true);
		}
	}
	
	private void maybeShowContextMenu(MouseEvent e) {
		if (e.isPopupTrigger()) {
			LevelPieceImageLabel levelPiece = this.getPieceAtLocation(e.getPoint());
			JPopupMenu levelEditContextMenu = new JPopupMenu();
			if (levelPiece.getIsItemDrop()) {
				EditItemDropsDialog blockEditDlg = new EditItemDropsDialog(this.levelEditWindow, levelPiece);
				JMenuItem blockPropertiesItem = new JMenuItem("Block properties...");
				blockPropertiesItem.addActionListener(new ContextMenuListener(blockEditDlg));
				
				// Add to the context menu for the level display panel
				levelEditContextMenu.add(blockPropertiesItem);	
			}
			else {
				Set<Character> allowableIDs = null;
				if (levelPiece.getIsPortal()) {
					allowableIDs = this.portalIDs;
				}
				else if (levelPiece.getIsTesla()) {
					allowableIDs = this.teslaIDs;
				}
				
				LevelPieceEditDialog blockEditDlg = new LevelPieceEditDialog(this.levelEditWindow, levelPiece, allowableIDs);
				JMenuItem blockPropertiesItem = new JMenuItem("Block properties...");
				blockPropertiesItem.addActionListener(new ContextMenuListener(blockEditDlg));
				
				// Add to the context menu for the level display panel
				levelEditContextMenu.add(blockPropertiesItem);
			}
			
			// Blocks all can have trigger IDs...
			JMenuItem triggerIDItem = new JMenuItem("Trigger ID...");
			triggerIDItem.addActionListener(new ContextMenuListener(new LevelPieceTriggerIDDialog(this.levelEditWindow, levelPiece)));
			levelEditContextMenu.add(triggerIDItem);
			
			levelEditContextMenu.show(e.getComponent(), e.getX(), e.getY());
		}
	}
	
	@Override
	public void mousePressed(MouseEvent e) {
		if (e.getButton() == MouseEvent.BUTTON1 && !e.isPopupTrigger()) {
			this.paintPieces(e.getPoint());
			this.paintingPieces = true;
		}
		else {
			this.paintingPieces = false;
		}
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		this.maybeShowContextMenu(e);
		this.paintingPieces = false;
	}

	@Override
	public void internalFrameActivated(InternalFrameEvent arg0) {
		this.levelEditWindow.updateButtons();
	}

	@Override
	public void internalFrameClosed(InternalFrameEvent arg0) {
		this.levelEditWindow.updateButtons();
	}

	@Override
	public void internalFrameClosing(InternalFrameEvent e) {
		this.closeAndCheckForSave();
	}
	
	/**
	 * Checks to make sure the user saves their document before the window closes.
	 * @return true if the window will actually be closing, false otherwise.
	 */
	public boolean closeAndCheckForSave() {
		this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		if (this.savedFileOnDisk == null || !this.savedFileOnDisk.exists() || this.hasBeenModified) {
			boolean didSave = false;
			while (!didSave) {
				
				Object[] options = { "Yes", "No", "Cancel" };
				int result = JOptionPane.showOptionDialog(this.levelEditWindow, "Would you like to save " + this.fileName + " before closing?", "Save Before Closing?", JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.WARNING_MESSAGE, null, options, options[2]);
				if (result == JOptionPane.YES_OPTION) {
					didSave = this.save();
				}
				else if (result == JOptionPane.CANCEL_OPTION) {
					this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
					return false;
				}
				else {
					return true;
				}
			}
		}
		
		return true;
	}

	@Override
	public void internalFrameDeactivated(InternalFrameEvent arg0) {
		this.levelEditWindow.updateButtons();	
	}

	@Override
	public void internalFrameDeiconified(InternalFrameEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void internalFrameIconified(InternalFrameEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void internalFrameOpened(InternalFrameEvent arg0) {
		this.levelEditWindow.updateButtons();
	}
}
