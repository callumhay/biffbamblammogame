package bbbleveleditor;

import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;
import javax.swing.Box;
import javax.swing.ImageIcon;
import javax.swing.JInternalFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameListener;

import bbbleveleditor.BBBLevelEditMainWindow.ToolMode;

public class BBBLevelEditDocumentWindow extends JInternalFrame 
implements MouseMotionListener, MouseListener, InternalFrameListener {
	private static final long serialVersionUID = 1L;
	
	private BBBLevelEditMainWindow levelEditWindow;
	private String fileName;
	private int currWidth;
	private int currHeight;
	private JPanel levelDisplayPanel;				// panel where the level is displayed
	private ArrayList<ArrayList<LevelPieceImageLabel>> pieces;	// access to the level piece names in this document [row (starting at the top)][col]
	
	public BBBLevelEditDocumentWindow(BBBLevelEditMainWindow window, String fileName, int width, int height) {
		super(fileName + " (" + width + "x" + height + ")", true, true, true, true);
		
		this.levelEditWindow = window;
		this.fileName = fileName;
		this.currWidth = width;
		this.currHeight = height;
		
		// Create the level display panel with its grid of level pieces...
		this.levelDisplayPanel = new JPanel(new GridLayout(height, width));
		this.levelDisplayPanel.setPreferredSize(new Dimension(width*LevelPiece.LEVEL_PIECE_WIDTH, 
															  height*LevelPiece.LEVEL_PIECE_HEIGHT));
		this.levelDisplayPanel.setMinimumSize(new Dimension(width*LevelPiece.LEVEL_PIECE_WIDTH, 
												            height*LevelPiece.LEVEL_PIECE_HEIGHT));
		this.levelDisplayPanel.setMaximumSize(new Dimension(width*LevelPiece.LEVEL_PIECE_WIDTH, 
	                                                        height*LevelPiece.LEVEL_PIECE_HEIGHT));		
		
		// Set all the pieces to the default piece and set the graphics in the panel
		// for those pieces as well
		this.pieces = new ArrayList<ArrayList<LevelPieceImageLabel>>(height);
		for (int i = 0; i < height; i++) {
			ArrayList<LevelPieceImageLabel> currRow = new ArrayList<LevelPieceImageLabel>(width);
			for (int j = 0; j < width; j++) {
				
				LevelPieceImageLabel tempLabel = new LevelPieceImageLabel(LevelPiece.DefaultPiece);
				tempLabel.setPreferredSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
				tempLabel.setMinimumSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
				tempLabel.setMaximumSize(new Dimension(LevelPiece.LEVEL_PIECE_WIDTH, LevelPiece.LEVEL_PIECE_HEIGHT));
				currRow.add(tempLabel);
				
				this.levelDisplayPanel.add(tempLabel);
			}
			this.pieces.add(i, currRow);
		}
		
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
		int x = Math.min(loc.x, this.currWidth*(LevelPiece.LEVEL_PIECE_WIDTH-1));
		x = Math.max(x, 0);
		int y = Math.min(loc.y, this.currHeight*(LevelPiece.LEVEL_PIECE_HEIGHT-1));
		y = Math.max(y, 0);
		
		// Now figure out the index of the piece to lookup in the 2D arraylist of pieces
		int colIndex = x / LevelPiece.LEVEL_PIECE_WIDTH;
		int rowIndex = y / LevelPiece.LEVEL_PIECE_HEIGHT;
		
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
		// TODO Auto-generated method stub
		
	}

	@Override
	public void internalFrameClosed(InternalFrameEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void internalFrameClosing(InternalFrameEvent arg0) {
		// TODO: save data...
	}

	@Override
	public void internalFrameDeactivated(InternalFrameEvent arg0) {
		// TODO Auto-generated method stub
		
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
		// TODO Auto-generated method stub
		
	}
}
