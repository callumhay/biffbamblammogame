package bbbleveleditor;

import java.awt.*;
import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.HashMap;

import javax.imageio.*;
import javax.swing.ImageIcon;

public class LevelPiece {
	
	public static LevelPiece DefaultPiece;
	public static HashMap<String, LevelPiece> LevelPieceCache = new HashMap<String, LevelPiece>();
	public static final int LEVEL_PIECE_WIDTH  = 32;
	public static final int LEVEL_PIECE_HEIGHT = 16;
	
	private String name;
	private String fileSymbol;
	private ImageIcon pieceImage;
	
	public LevelPiece(String name, String symbol, String fileName) {
		this.name = name;
		this.fileSymbol = symbol;
		try {
			Image image = ImageIO.read(new File(BBBLevelEditorMain.class.getResource("resources/" + fileName).toURI()));
			this.pieceImage = new ImageIcon(image);

		} catch (IOException e) {
			e.printStackTrace();
			this.pieceImage = null;
		} catch (URISyntaxException e) {
			e.printStackTrace();
			this.pieceImage = null;
		}
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
