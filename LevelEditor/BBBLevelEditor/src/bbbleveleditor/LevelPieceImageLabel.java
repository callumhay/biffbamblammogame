package bbbleveleditor;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ImageObserver;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeSet;

import javax.swing.ImageIcon;
import javax.swing.JLabel;

public class LevelPieceImageLabel extends JLabel {
	private static final long serialVersionUID = 1L;
	
	private static final Font portalFont = new Font(Font.SANS_SERIF, Font.PLAIN, 10);
	private static final Font teslaFont  = new Font(Font.SANS_SERIF, Font.PLAIN, 10);
	
	public static final char INVALID_ID = '\0';
	
	private Image image;
	private ImageObserver imageObserver; 
	private LevelPiece piece;

	private char blockID;
	private Set<Character> siblingIDs = new TreeSet<Character>();
	private boolean teslaBlockStartsOn;
	private boolean teslaIsChangable;
	
	private int cannonDegAngle1;
	private int cannonDegAngle2;
	private int triggerID;
	
	private ArrayList<String> itemDropTypes = new ArrayList<String>();
	
	private int switchTriggerID;	// When this level piece is a switch, this ID is the ID of what gets triggered by the switch
	
	static boolean IsValidBlockID(char id) {
		if ((id >= 'A' && id <= 'z') || (id >= '0' && id <= '9')) {
			return true;
		}
		return false;
	}
	
	public LevelPieceImageLabel(LevelPiece piece) {
		this.blockID = INVALID_ID;
		this.teslaBlockStartsOn = false;
		this.teslaIsChangable   = true;
		this.setLevelPiece(piece);
		this.cannonDegAngle1 = 0;
		this.cannonDegAngle2 = 0;
		this.itemDropTypes.add("all");
		this.triggerID = LevelPiece.NO_TRIGGER_ID;
		this.switchTriggerID = 0;
	}
	
	// constructor with icon
	public LevelPieceImageLabel(String pieceSymbol) throws Exception {
		String justTheSymbol = pieceSymbol;
		
		// Clean up in the case of a portal block - since it has variables in it
		if (pieceSymbol.length() == 6 && pieceSymbol.substring(0, 2).equals(LevelPiece.PORTAL_PIECE_SYMBOL + "(")) {
			this.blockID  = pieceSymbol.charAt(2);
			char siblingID = pieceSymbol.charAt(4);
			this.siblingIDs.add(siblingID);
			justTheSymbol = LevelPiece.PORTAL_PIECE_SYMBOL;
		}
		else if (pieceSymbol.length() >= 6 && pieceSymbol.substring(0, 2).equals(LevelPiece.TESLA_PIECE_SYMBOL + "(")) {
			String siblingStr = pieceSymbol.substring(2);
			String[] splitSiblingStr = siblingStr.split("[\\,)(]");
			for (int i = 0; i < splitSiblingStr.length; i++) {
				String currSiblingID = splitSiblingStr[i];
				currSiblingID.trim();
				if (currSiblingID.length() > 1) {
					break;
				}
				else if (currSiblingID.length() == 1){
					if (i == 0) {
						this.teslaBlockStartsOn = (currSiblingID.charAt(0) == '1') ? true : false;
					}
					else if (i == 1) {
						this.teslaIsChangable = (currSiblingID.charAt(0) == '1') ? true : false;
					}
					else if (i == 2) {
						this.blockID = currSiblingID.charAt(0);
					}
					else {
						this.siblingIDs.add(currSiblingID.charAt(0));
					}
				}
			}
			justTheSymbol = LevelPiece.TESLA_PIECE_SYMBOL;
		}
		else if (pieceSymbol.length() >= 3 &&  pieceSymbol.substring(0, 2).equals(LevelPiece.ITEM_DROP_PIECE_SYMBOL + "(")) {
			String[] itemDropTypeNames = pieceSymbol.substring(2, pieceSymbol.length()).split("[\\(,\\)]");
			for (int i = 0; i < itemDropTypeNames.length; i++) {
				if (itemDropTypeNames[i].length() > 1) {
					this.itemDropTypes.add(itemDropTypeNames[i]);
				}
			}
			
			justTheSymbol = LevelPiece.ITEM_DROP_PIECE_SYMBOL;
		}
		else if (pieceSymbol.length() >= 3 &&  pieceSymbol.substring(0, 2).equals(LevelPiece.CANNON_PIECE_SYMBOL + "(")) {

			String angleValue1, angleValue2;
			int indexOfHyphen = pieceSymbol.indexOf('-');
			
			if (indexOfHyphen != -1 && indexOfHyphen != 2) {
				angleValue1 = pieceSymbol.substring(2, indexOfHyphen);
				angleValue2 = pieceSymbol.substring(indexOfHyphen+1, pieceSymbol.length()-1);
			}
			else {
				angleValue1 = pieceSymbol.substring(2, pieceSymbol.length()-1);
				angleValue2 = angleValue1;
			}
			
			this.setCannonBlockDegAngle1(Integer.parseInt(angleValue1));
			this.setCannonBlockDegAngle2(Integer.parseInt(angleValue2));
			justTheSymbol = LevelPiece.CANNON_PIECE_SYMBOL;
		}
		else if (pieceSymbol.length() >= 3 && pieceSymbol.substring(0, 2).equals(LevelPiece.SWITCH_PIECE_SYMBOL + "(")) {
			String switchTriggerIDStr = pieceSymbol.substring(2, pieceSymbol.length()-1);
			this.switchTriggerID = Integer.parseInt(switchTriggerIDStr);
			justTheSymbol = LevelPiece.SWITCH_PIECE_SYMBOL;
		}

		// Check for a trigger ID...
		int triggerIDStart = pieceSymbol.indexOf('{');
		if (triggerIDStart != -1) {
			String triggerIDStr = pieceSymbol.substring(triggerIDStart+1, pieceSymbol.length()-1);
			this.triggerID = Integer.parseInt(triggerIDStr);
		}
		else {
			this.triggerID = LevelPiece.NO_TRIGGER_ID;
		}
		
		LevelPiece piece = LevelPiece.LevelPieceCache.get(justTheSymbol);
		this.setLevelPiece(piece);
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
	public boolean getIsTeslaOnBool() {
		return this.teslaBlockStartsOn;
	}
	public void setTeslaStartsOn(boolean startsOn) {
		this.teslaBlockStartsOn = startsOn;
	}
	
	public int getTriggerID() {
		return this.triggerID;
	}
	public void setTriggerID(int triggerID) {
		this.triggerID = triggerID;
	}
	public int getSwitchTriggerID() {
		return this.switchTriggerID;
	}
	public void setSwitchTriggerID(int id) {
		this.switchTriggerID = id;
	}
	
	
	public char getIsTeslaChangable() {
		return this.teslaIsChangable ? '1' : '0';
	}
	public boolean getIsTeslaChangableBool() {
		return this.teslaIsChangable;
	}
	void setTeslaChangable(boolean changable) {
		this.teslaIsChangable = changable;
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
	public boolean getIsItemDrop() {
		return piece.getSymbol().equals(LevelPiece.ITEM_DROP_PIECE_SYMBOL);
	}
	public boolean getIsCannonBlock() {
		return piece.getSymbol().equals(LevelPiece.CANNON_PIECE_SYMBOL);
	}
	public boolean getIsSwitchBlock() {
		return piece.getSymbol().equals(LevelPiece.SWITCH_PIECE_SYMBOL);
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
	
	public ArrayList<String> getItemDropTypes() {
		return this.itemDropTypes;
	}
	public void setItemDropTypes(ArrayList<String> dropTypes) {
		this.itemDropTypes = dropTypes;
	}
	public void addItemDropType(String itemName) {
		this.itemDropTypes.add(itemName);
	}
	
	
	public int getCannonBlockDegAngle1() {
		return this.cannonDegAngle1;
	}
	public int getCannonBlockDegAngle2() {
		return this.cannonDegAngle2;
	}
	public void setCannonBlockDegAngle1(int angle) {
		if (angle == -1) {
			this.cannonDegAngle1 = 0;
			this.cannonDegAngle2 = 359;
		}
		else {
			this.cannonDegAngle1 = angle;
		}
	}
	public void setCannonBlockDegAngle2(int angle) {
		if (angle == -1) {
			this.cannonDegAngle1 = 0;
			this.cannonDegAngle2 = 359;
		}
		else {
			this.cannonDegAngle2 = angle;
		}
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
			
			//this.setToolTipText("Portal block: " + portalString);
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
			String onOffString = this.teslaBlockStartsOn ? "On" : "Off";
			g.drawString(onOffString, 2, 2);
			
			//this.setToolTipText("Tesla block: " + onOffString + ", " + teslaString);
		}

		
	}
}
