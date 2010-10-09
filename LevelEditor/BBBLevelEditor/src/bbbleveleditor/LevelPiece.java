package bbbleveleditor;

import java.util.HashMap;
import javax.swing.ImageIcon;

public class LevelPiece {
	public static final String PORTAL_PIECE_NAME   = "Portal";
	public static final String PORTAL_PIECE_SYMBOL = "X";
	public static final String TESLA_PIECE_NAME    = "Tesla";
	public static final String TESLA_PIECE_SYMBOL  = "A";
	public static final String ITEM_DROP_PIECE_SYMBOL = "D";
	public static final String ITEM_DROP_PIECE_NAME   = "Item Drop";
	
	public static LevelPiece DefaultPiece;
	
	// Hash mapping of the symbols to level pieces
	public static HashMap<String, LevelPiece> LevelPieceCache = new HashMap<String, LevelPiece>();
	
	public static final int LEVEL_PIECE_WIDTH  = 32;
	public static final int LEVEL_PIECE_HEIGHT = 16;
	
	private String name;
	private String fileSymbol;
	private ImageIcon pieceImage;
	
	public LevelPiece(String name, String symbol, String fileName) {
		this.name = name;
		this.fileSymbol = symbol;
		this.pieceImage = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/" + fileName));
	}
	
	public String getName() {
		return this.name;
	}
	public ImageIcon getImageIcon() {
		return this.pieceImage;
	}
	public String getSymbol() {
		return this.fileSymbol;
	}
}
