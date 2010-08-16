package bbbleveleditor;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ImageObserver;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeSet;

import javax.swing.ImageIcon;
import javax.swing.JLabel;

public class LevelPieceImageLabel extends JLabel {
	private static final long serialVersionUID = 1L;
	
	private static final Font portalFont = new Font(Font.SANS_SERIF, Font.BOLD, 20);
	private static final Font teslaFont  = new Font(Font.SANS_SERIF, Font.BOLD, 14);
	
	public static final char INVALID_ID = '\0';
	
	private Image image;
	private ImageObserver imageObserver; 
	private LevelPiece piece;

	private char blockID;
	private Set<Character> siblingIDs = new TreeSet<Character>();
	private boolean teslaBlockStartsOn;
	
	static boolean IsValidBlockID(char id) {
		if ((id >= 'A' && id <= 'z') || (id >= '0' && id <= '9')) {
			return true;
		}
		return false;
	}
	
	public LevelPieceImageLabel(LevelPiece piece) {
		this.blockID = INVALID_ID;
		this.teslaBlockStartsOn = false;
		this.setLevelPiece(piece);
	}
	
	// constructor with icon
	public LevelPieceImageLabel(String pieceSymbol) throws Exception {
		// Clean up in the case of a portal block - since it has variables in it
		if (pieceSymbol.length() == 6 && pieceSymbol.substring(0, 2).equals(LevelPiece.PORTAL_PIECE_SYMBOL + "(")) {
			this.blockID  = pieceSymbol.charAt(2);
			char siblingID = pieceSymbol.charAt(4);
			this.siblingIDs.add(siblingID);
			pieceSymbol = LevelPiece.PORTAL_PIECE_SYMBOL;
		}
		else if (pieceSymbol.length() >= 6 && pieceSymbol.substring(0, 2).equals(LevelPiece.TESLA_PIECE_SYMBOL + "(")) {
			String siblingStr = pieceSymbol.substring(2);
			String[] splitSiblingStr = siblingStr.split("[\\,)(]");
			for (int i = 0; i < splitSiblingStr.length; i++) {
				String currSiblingID = splitSiblingStr[i];
				currSiblingID.trim();
				if (currSiblingID.length() > 1) {
					throw new Exception("Badly formed level file document.");
				}
				else if (currSiblingID.length() == 1){
					if (i == 0) {
						teslaBlockStartsOn = (currSiblingID.charAt(0) == '1') ? true : false;
					}
					else if (i == 1) {
						this.blockID = currSiblingID.charAt(0);
					}
					else {
						this.siblingIDs.add(currSiblingID.charAt(0));
					}
				}
			}
			pieceSymbol = LevelPiece.TESLA_PIECE_SYMBOL;
		}
		
		this.setLevelPiece(LevelPiece.LevelPieceCache.get(pieceSymbol));
	}

	public void setLevelPiece(LevelPiece piece) {
		assert(piece != null);
		ImageIcon icon = piece.getImageIcon();
		this.image = icon.getImage();
		this.imageObserver = icon.getImageObserver();
		this.piece = piece;
		
		this.invalidate();
		this.repaint();
	}
	
	public LevelPiece getLevelPiece() {
		return this.piece;
	}
	
	public char getIsTeslaOn() {
		return this.teslaBlockStartsOn ? '1' : '0';
	}
	
	public boolean getIsValid() {
		if (this.getIsPortal() || this.getIsTesla()) {
			if (!LevelPieceImageLabel.IsValidBlockID(this.getBlockID())) {
				return false;
			}
			
			Iterator<Character> iter = this.siblingIDs.iterator();
			while (iter.hasNext()) {
				if (!LevelPieceImageLabel.IsValidBlockID(iter.next())) {
					return false;
				}
			}
		}
		return true;
	}
	public boolean getHasIDsInGivenSet(Set<Character> ids) {
		if (this.getIsPortal() || this.getIsTesla()) {
			if (!ids.contains(this.getBlockID())) {
				return false;
			}
			Iterator<Character> iter = this.siblingIDs.iterator();
			while (iter.hasNext()) {
				if (!ids.contains(iter.next())) {
					return false;
				}
			}
		}
		return true;
	}
	
	
	public boolean getIsPortal() {
		return piece.getSymbol().equals(LevelPiece.PORTAL_PIECE_SYMBOL);
	}
	public boolean getIsTesla() {
		return piece.getSymbol().equals(LevelPiece.TESLA_PIECE_SYMBOL);
	}
	
	public void setBlockID(char blockID) {
		this.blockID = blockID;
		this.invalidate();
		this.repaint();
	}
	public void setSiblingIDs(Set<Character> siblingIDs) {
		this.siblingIDs = siblingIDs;
		this.invalidate();
		this.repaint();
	}
	public char getBlockID() {
		return this.blockID;
	}
	public char getTeslaID() {
		return this.blockID;
	}
	public void setPortalSiblingID(char siblingID) {
		this.siblingIDs.clear();
		this.siblingIDs.add(siblingID);
		this.invalidate();
		this.repaint();		
	}
	public Set<Character> getSiblingIDs() {
		return this.siblingIDs;
	}
	public char getPortalSiblingID() {
		if (this.siblingIDs.size() == 1) {
			return this.siblingIDs.iterator().next();
		}
		return INVALID_ID;
			
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
		if (this.getIsPortal()) {
			String portalString = this.getBlockID() + " : " + this.getPortalSiblingID();
			g.setFont(LevelPieceImageLabel.portalFont);
			g.setColor(Color.BLACK);
			g.drawString(portalString, 4, this.getHeight()/2 + 2);
			g.setColor(Color.CYAN);
			g.drawString(portalString, 2, this.getHeight()/2);
		}
		else if (this.getIsTesla()) {
			String teslaString = this.getTeslaID() + " : (";
			Iterator<Character> iter = this.siblingIDs.iterator();
			while (iter.hasNext()) {
				teslaString = teslaString + iter.next();
				if (iter.hasNext()) {
					teslaString = teslaString + ",";
				}
			}
			teslaString = teslaString + ")";
			g.setFont(LevelPieceImageLabel.teslaFont);
			g.setColor(Color.BLACK);
			g.drawString(teslaString, 4, this.getHeight()/2 + 2);
			g.setColor(Color.CYAN);
			g.drawString(teslaString, 2, this.getHeight()/2);
			
			g.drawString(this.teslaBlockStartsOn ? "On" : "Off", 2, 2);
		}
	}
}