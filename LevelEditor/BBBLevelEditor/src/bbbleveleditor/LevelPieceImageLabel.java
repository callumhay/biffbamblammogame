package bbbleveleditor;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ImageObserver;

import javax.swing.ImageIcon;
import javax.swing.JLabel;

public class LevelPieceImageLabel extends JLabel {
	private static final long serialVersionUID = 1L;
	
	private static final Font portalFont = new Font(Font.SANS_SERIF, Font.BOLD, 20);
	
	public static final char INVALID_PORTAL_ID = '\0';
	
	Image image;
	ImageObserver imageObserver; 
	LevelPiece piece;

	boolean isPortal;
	char portalID;
	char siblingID;
	
	public LevelPieceImageLabel(LevelPiece piece) {
		this.portalID = INVALID_PORTAL_ID;
		this.siblingID = INVALID_PORTAL_ID;
		this.setLevelPiece(piece);
	}
	
	// constructor with icon
	public LevelPieceImageLabel(String pieceSymbol) {
		// Clean up in the case of a portal block - since it has variables in it
		if (pieceSymbol.length() == 6 && pieceSymbol.substring(0, 2).equals("X(")) {
			this.portalID  = pieceSymbol.charAt(2);
			this.siblingID = pieceSymbol.charAt(4);
			pieceSymbol = "X";
		}
		this.setLevelPiece(LevelPiece.LevelPieceCache.get(pieceSymbol));
	}

	public void setLevelPiece(LevelPiece piece) {
		assert(piece != null);
		ImageIcon icon = piece.getImageIcon();
		this.image = icon.getImage();
		this.imageObserver = icon.getImageObserver();
		this.piece = piece;
		
		if (piece.getSymbol().equals(LevelPiece.PORTAL_PIECE_SYMBOL)) {
			this.isPortal = true;
		}
		else {
			this.isPortal = false;
		}
		
		this.invalidate();
		this.repaint();
	}
	
	public LevelPiece getLevelPiece() {
		return this.piece;
	}
	
	public boolean getIsPortal() {
		return this.isPortal;
	}
	public void setPortalID(char portalID) {
		this.portalID = portalID;
		this.invalidate();
		this.repaint();
	}
	public char GetPortalID() {
		return this.portalID;
	}
	public void setSiblingID(char siblingID) {
		this.siblingID = siblingID;
		this.invalidate();
		this.repaint();		
	}
	public char getSiblingID() {
		return this.siblingID;
	}
	// overload setIcon method
	void setIcon(ImageIcon icon) {
		image = icon.getImage();
		imageObserver = icon.getImageObserver();
	}
	
	// overload paint()
	public void paint(Graphics g) {
		super.paint(g);
		g.drawImage(image, 0, 0, this.getWidth(), this.getHeight(), this.imageObserver);
		if (this.isPortal) {
			String portalString = this.portalID + " : " + this.siblingID;
			g.setFont(LevelPieceImageLabel.portalFont);
			g.setColor(Color.BLACK);
			g.drawString(portalString, 2, this.getHeight()/2 + 2);
			g.setColor(Color.CYAN);
			g.drawString(portalString, 0, this.getHeight()/2);
		}	
	}
}
