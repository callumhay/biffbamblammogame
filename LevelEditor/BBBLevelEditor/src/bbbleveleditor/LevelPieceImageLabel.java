package bbbleveleditor;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ImageObserver;

import java.util.HashMap;
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
	
	private boolean portalFlipsPaddle;
	
	private int cannonDegAngle1;
	private int cannonDegAngle2;
	private int triggerID;
	
	private double warpPortalStartTime;
	private double warpPortalEndTime;
	private String warpPortalWorldName;
	private int warpPortalLevelNum;
	
	private HashMap<String, ItemDropSettings> itemDropTypes = new HashMap<String, ItemDropSettings>();
	
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
		this.portalFlipsPaddle  = false;
		this.setLevelPiece(piece);
		this.cannonDegAngle1 = 0;
		this.cannonDegAngle2 = 0;
		this.triggerID = LevelPiece.NO_TRIGGER_ID;
		this.switchTriggerID = 0;
		
		this.warpPortalStartTime = 0.0;
		this.warpPortalEndTime   = -1.0;
		this.warpPortalWorldName = "surrealism_dada";
		this.warpPortalLevelNum = 1;
	}
	
	// constructor with icon
	public LevelPieceImageLabel(String pieceSymbol) throws Exception {
		String justTheSymbol = pieceSymbol;
		
		this.triggerID = LevelPiece.NO_TRIGGER_ID;
		this.switchTriggerID = 0;
		this.cannonDegAngle1 = 0;
		this.cannonDegAngle2 = 0;
		
		this.warpPortalStartTime = 0.0;
		this.warpPortalEndTime   = -1.0;
		this.warpPortalLevelNum  = 1;
		this.warpPortalWorldName = "surrealism_dada";
		
		// Clean up in the case of a portal block - since it has variables in it
		if (pieceSymbol.length() >= 6 && pieceSymbol.substring(0, 2).equals(LevelPiece.PORTAL_PIECE_SYMBOL + "(")) {
			this.blockID  = pieceSymbol.charAt(2);
			char siblingID = pieceSymbol.charAt(4);
			this.siblingIDs.add(siblingID);
			
			this.portalFlipsPaddle = false;
			if (pieceSymbol.length() >= 8) {
				this.portalFlipsPaddle = (pieceSymbol.substring(5).contains("f"));
			}
			
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
		else if (pieceSymbol.length() >= 3 &&
				(pieceSymbol.substring(0, 2).equals(LevelPiece.ITEM_DROP_PIECE_SYMBOL + "(") ||
				 pieceSymbol.substring(0, 2).equals(LevelPiece.ALWAYS_DROP_PIECE_SYMBOL + "("))) {
			
			String[] itemDropTypeNames = pieceSymbol.substring(2, pieceSymbol.length()).split("[\\(,\\) ]");
			this.itemDropTypes.clear();
			for (int i = 0; i < itemDropTypeNames.length; i++) {
				
				String currStr = itemDropTypeNames[i];
				
				if (currStr.length() > 0 && !currStr.contains("{") && !currStr.contains("}") && !currStr.contains("(") && !currStr.contains(")")) {
					
					// Check to see if the value is a '*' meaning that the item will only drop if it's unlocked
					boolean onlyDropIfUnlocked = false;
					if (currStr.equals("*")) {
						onlyDropIfUnlocked = true;
						if (i == itemDropTypeNames.length-1) {
							assert(false);
							break;
						}
						else {
							i++;
							currStr = itemDropTypeNames[i];
						}
					}
					
					String currItemDropName = currStr;
					
					// Check to see if there's a probability in the next string token
					int likelihoodValue = 1;
					if (i < itemDropTypeNames.length-1) {
						try {
							likelihoodValue = Math.min(3, Math.max(0, Integer.parseInt(itemDropTypeNames[i+1])));
							i++;
						}
						catch (NumberFormatException e) {
						}
					}
					
					ItemDropSettings currSettings = this.itemDropTypes.get(currItemDropName);
					if (currSettings == null) {
						this.itemDropTypes.put(currItemDropName, new ItemDropSettings(likelihoodValue, onlyDropIfUnlocked));
					}
					else {
						currSettings.IncrementProbability();
					}
				}
			}
			
			justTheSymbol = pieceSymbol.substring(0,1);
		}
		else if (pieceSymbol.length() >= 3 && pieceSymbol.substring(0, 2).equals(LevelPiece.CANNON_PIECE_SYMBOL + "(")) {
			this.ReadCannonInfo(pieceSymbol, 2);
			justTheSymbol = LevelPiece.CANNON_PIECE_SYMBOL;
		}
		else if (pieceSymbol.length() >= 3 && pieceSymbol.substring(0, 3).equals(LevelPiece.ONE_SHOT_CANNON_PIECE_SYMBOL + "(")) {
			this.ReadCannonInfo(pieceSymbol, 3);
			justTheSymbol = LevelPiece.ONE_SHOT_CANNON_PIECE_SYMBOL;
		}
		else if (pieceSymbol.length() >= 3 && pieceSymbol.substring(0, 2).equals(LevelPiece.SWITCH_PIECE_SYMBOL + "(")) {
			int endSubstringIdx;
			if (pieceSymbol.charAt(pieceSymbol.length()-1) == '}') {
				endSubstringIdx = pieceSymbol.indexOf('{');
			}
			else {
				endSubstringIdx = pieceSymbol.length()-1;
			}
			
			String switchTriggerIDStr = pieceSymbol.substring(2, endSubstringIdx);
			this.switchTriggerID = Integer.parseInt(switchTriggerIDStr);
			justTheSymbol = LevelPiece.SWITCH_PIECE_SYMBOL;
		}
		else if (pieceSymbol.length() >= 6 && pieceSymbol.substring(0, 2).equals(LevelPiece.WARP_PORTAL_PIECE_SYMBOL + "(")) {
			
			String[] parameterStrs = pieceSymbol.substring(2, pieceSymbol.length()).split("[\\(,\\)]");
			
			if (parameterStrs.length == 4) {
				this.setWarpPortalStartTime(Double.parseDouble(parameterStrs[0]));
				this.setWarpPortalEndTime(Double.parseDouble(parameterStrs[1]));
				this.setWarpPortalWorldName(parameterStrs[2]);
				this.setWarpPortalLevelNum(Integer.parseInt(parameterStrs[3]));
			}
			else {
				assert(false);
			}
			
			justTheSymbol = LevelPiece.WARP_PORTAL_PIECE_SYMBOL;
		}

		LevelPiece piece = LevelPiece.LevelPieceCache.get(justTheSymbol);
		this.setLevelPiece(piece);
		
		// Check for a trigger ID...
		int triggerIDStart = pieceSymbol.indexOf('{');
		if (triggerIDStart != -1) {
			String triggerIDStr = pieceSymbol.substring(triggerIDStart+1, pieceSymbol.length()-1);
			this.triggerID = Integer.parseInt(triggerIDStr);
		}
		else {
			this.triggerID = LevelPiece.NO_TRIGGER_ID;
		}
	}

	private void ReadCannonInfo(String pieceSymbol, int startIdx) {
		String angleValue1, angleValue2;
		int indexOfHyphen = pieceSymbol.indexOf('-');
		
		if (indexOfHyphen != -1 && indexOfHyphen != startIdx) {
			angleValue1 = pieceSymbol.substring(startIdx, indexOfHyphen);
			angleValue2 = pieceSymbol.substring(indexOfHyphen+1, pieceSymbol.length()-1);
		}
		else {
			angleValue1 = pieceSymbol.substring(startIdx, pieceSymbol.length()-1);
			angleValue2 = angleValue1;
		}
		
		this.setCannonBlockDegAngle1(Integer.parseInt(angleValue1));
		this.setCannonBlockDegAngle2(Integer.parseInt(angleValue2));
	}
	
	public void setLevelPiece(LevelPiece piece) {
		assert(piece != null);
		ImageIcon icon = piece.getImageIcon();
		this.image = icon.getImage();
		this.imageObserver = icon.getImageObserver();
		this.piece = piece;
		this.triggerID = LevelPiece.NO_TRIGGER_ID;
		
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
	
	public boolean getPortalFlipsPaddle() {
		return this.portalFlipsPaddle;
	}
	void setPortalFlipsPaddle(boolean flipsPaddle) {
		this.portalFlipsPaddle = flipsPaddle;
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
	public boolean getIsAlwaysDrop() {
		return piece.getSymbol().equals(LevelPiece.ALWAYS_DROP_PIECE_SYMBOL);
	}
	public boolean getIsCannonBlock() {
		return piece.getSymbol().equals(LevelPiece.CANNON_PIECE_SYMBOL);
	}
	public boolean getIsOneShotCannonBlock() {
		return piece.getSymbol().equals(LevelPiece.ONE_SHOT_CANNON_PIECE_SYMBOL);
	}
	public boolean getIsSwitchBlock() {
		return piece.getSymbol().equals(LevelPiece.SWITCH_PIECE_SYMBOL);
	}
	public boolean getIsWarpPortal() {
		return piece.getSymbol().equals(LevelPiece.WARP_PORTAL_PIECE_SYMBOL);
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
	
	public HashMap<String, ItemDropSettings> getItemDropTypes() {
		return this.itemDropTypes;
	}
	public void setItemDropTypes(HashMap<String, ItemDropSettings> dropTypes) {
		this.itemDropTypes = dropTypes;
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
	
	public double getWarpPortalStartTime() {
		return this.warpPortalStartTime;
	}
	public double getWarpPortalEndTime() {
		return this.warpPortalEndTime;
	}
	public String getWarpPortalWorldName() {
		return this.warpPortalWorldName;
	}
	public int getWarpPortalLevelNum() {
		return this.warpPortalLevelNum;
	}
	void setWarpPortalStartTime(double startTime) {
		this.warpPortalStartTime = startTime;
	}
	void setWarpPortalEndTime(double endTime) {
		this.warpPortalEndTime = endTime;
	}
	void setWarpPortalWorldName(String name) {
		this.warpPortalWorldName = name;
	}
	void setWarpPortalLevelNum(int num) {
		this.warpPortalLevelNum = num;
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
