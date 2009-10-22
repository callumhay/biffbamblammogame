package bbbleveleditor;

import java.awt.*;
import java.awt.image.*;
import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;

import javax.imageio.*;

public class LevelPiece {
	private String name;
	private String fileSymbol;
	private BufferedImage pieceImage;
	
	public LevelPiece(String name, String symbol, String fileName) {
		this.name = name;
		this.fileSymbol = symbol;
		try {
			this.pieceImage = ImageIO.read(new File(BBBLevelEditorMain.class.getResource("resources/" + fileName).toURI()));
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
	public Image getImage() {
		return this.pieceImage;
	}
}
