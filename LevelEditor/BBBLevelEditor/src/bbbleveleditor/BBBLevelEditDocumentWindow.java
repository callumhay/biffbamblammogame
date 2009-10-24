package bbbleveleditor;

import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Reader;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.prefs.Preferences;
import java.util.regex.Pattern;

import javax.swing.Box;
import javax.swing.ImageIcon;
import javax.swing.JFileChooser;
import javax.swing.JInternalFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameListener;

import bbbleveleditor.BBBLevelEditMainWindow.ToolMode;

public class BBBLevelEditDocumentWindow extends JInternalFrame 
implements MouseMotionListener, MouseListener, InternalFrameListener {
	private static final long serialVersionUID = 1L;
	
	private BBBLevelEditMainWindow levelEditWindow = null;
	private String fileName = "";
	private int currWidth = -1;
	private int currHeight = -1;
	private File savedFileOnDisk = null;
	
	private JPanel levelDisplayPanel; // panel where the level is displayed
	
	// access to the level piece names in this document [row (starting at the top)][col]
	private ArrayList<ArrayList<LevelPieceImageLabel>> pieces;	
	
	public BBBLevelEditDocumentWindow(BBBLevelEditMainWindow window) {
		super("", true, true, true, true);
		
		this.levelEditWindow = window;
		this.fileName = "";
		this.currWidth = 0;
		this.currHeight = 0;
		this.pieces = null;
	}	
	
	public BBBLevelEditDocumentWindow(BBBLevelEditMainWindow window, String fileName, int width, int height) {
		super(fileName + " (" + width + "x" + height + ")", true, true, true, true);
		
		this.levelEditWindow = window;
		this.fileName = fileName;
		this.currWidth = width;
		this.currHeight = height;
		
		this.setupUI();
		this.initDefaultPieces();
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
	
	public void setFileName(String fileName) {
		this.fileName = fileName;
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
		openDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);
		openDlg.setFileHidingEnabled(true);
		openDlg.setMultiSelectionEnabled(false);
		
		String openDir = prefs.get("lastOpenLocation", "");
		if (!openDir.isEmpty()) {
			openDlg.setSelectedFile(new File(openDir));
		}
		
		openDlg.setFileFilter(new BBBLevelFileFilter());
		int result = openDlg.showOpenDialog(this.levelEditWindow);
		
		if (result == JFileChooser.APPROVE_OPTION) {
			
			String openDirToSave = openDlg.getSelectedFile().getPath();
			prefs.put("lastOpenLocation", openDirToSave);
			
			this.savedFileOnDisk = openDlg.getSelectedFile();
			if (this.savedFileOnDisk == null) {
				assert(false);
				return false;
			}
			
			Scanner levelFileScanner = null;
			try {
				levelFileScanner = new Scanner(this.savedFileOnDisk);
				
				// Start by reading the width and height of the level
				boolean goodFormat = levelFileScanner.hasNextInt();
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
				//levelFileScanner.useDelimiter(Pattern.compile("\\s"));
				String currLevelPieceSymbol = "";
				LevelPiece currLevelPiece = null;
				
				this.setupUI();
				
				this.pieces = new ArrayList<ArrayList<LevelPieceImageLabel>>(this.currHeight);
				for (int row = 0; row < this.currHeight; row++) {
					ArrayList<LevelPieceImageLabel> currRow = new ArrayList<LevelPieceImageLabel>(this.currWidth);
					
					for (int col = 0; col < this.currWidth; col++) {
						
						currLevelPieceSymbol = levelFileScanner.next();
						currLevelPiece = LevelPiece.LevelPieceCache.get(currLevelPieceSymbol);
						assert(currLevelPiece != null);
						
						LevelPieceImageLabel tempLabel = new LevelPieceImageLabel(currLevelPiece);
						
						currRow.add(tempLabel);
						this.levelDisplayPanel.add(tempLabel);
					}
					
					this.pieces.add(row, currRow);
				}
				this.pack();
				
				this.fileName = this.savedFileOnDisk.getName();
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
		return true;
	}
	
	public void save() {
		if (this.savedFileOnDisk == null) {
			this.saveAs();
		}
		else if (!this.savedFileOnDisk.canWrite()) {
			JOptionPane.showMessageDialog(this, "Could not write to disk, please save to a writable file.", "Could not write", JOptionPane.ERROR_MESSAGE);
			this.saveAs();
		}
		this.saveToFile();
	}
	
	public void saveAs() {
		// Use preferences to store and retrieve the last save location
		Preferences prefs = Preferences.userRoot().node(this.getClass().getName());	
		
		JFileChooser saveAsDlg = new JFileChooser();
		
		String saveAsDir = prefs.get("lastSaveAsLocation", "");
		if (!saveAsDir.isEmpty()) {
			saveAsDlg.setSelectedFile(new File(saveAsDir));
		}	
		
		int result = saveAsDlg.showSaveDialog(this.levelEditWindow);
		
		if (result == JFileChooser.APPROVE_OPTION) {
			String saveDirLoc = saveAsDlg.getSelectedFile().getPath();
			prefs.put("lastSaveAsLocation", saveDirLoc);	
			
			this.savedFileOnDisk = saveAsDlg.getSelectedFile();
			this.saveToFile();
		}
	}
	
	private void saveToFile() {
		// The savedFileOnDisk is not null and can be used to save the current
		// state of this level edit document
		if (this.savedFileOnDisk == null) {
			assert(false);
			return;
		}
		
		Writer levelFileWriter = null;
		try {
			levelFileWriter = new FileWriter(this.savedFileOnDisk);
			
			// File starts with the width and height of the level...
			levelFileWriter.write(this.currWidth + " " + this.currHeight + "\r\n");
			
			// Followed by the level pieces...
			for (int row = 0; row < this.currHeight; row++) {
				for (int col = 0; col < this.currWidth; col++) {
					LevelPiece currLvlPiece = this.pieces.get(row).get(col).getLevelPiece();
					assert(currLvlPiece != null);
					
					levelFileWriter.write(currLvlPiece.getSymbol() + " ");
				}
				levelFileWriter.write("\r\n");
			}
			
		} 
		catch (Exception e) {
			JOptionPane.showMessageDialog(this, "Failed to write to disk.", "Could not write", JOptionPane.ERROR_MESSAGE);
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
		this.paintPieces(e.getPoint());
	}

	@Override
	public void mouseMoved(MouseEvent e) {
	}
	
	public void changeLevelPieceAtLocation(Point loc, LevelPiece newPiece) {
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
		LevelPieceImageLabel editPiece = levelPieceRow.get(colIndex);
		editPiece.setLevelPiece(newPiece);
	}

	@Override
	public void mouseClicked(MouseEvent e) {
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

	@Override
	public void mousePressed(MouseEvent e) {
		this.paintPieces(e.getPoint());
		
	}

	@Override
	public void mouseReleased(MouseEvent arg0) {
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
	public void internalFrameClosing(InternalFrameEvent arg0) {
		// TODO: save data...
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
