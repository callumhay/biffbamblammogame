package bbbleveleditor;

import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ImageObserver;

import javax.swing.ImageIcon;
import javax.swing.JLabel;

public class LevelPieceImageLabel extends JLabel {
	private static final long serialVersionUID = 1L;
	
	Image image;
	ImageObserver imageObserver; 
	LevelPiece piece;

	// constructor with icon
	public LevelPieceImageLabel(LevelPiece piece) {
		this.setLevelPiece(piece);
	}

	public void setLevelPiece(LevelPiece piece) {
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
	
	// overload setIcon method
	void setIcon(ImageIcon icon) {
		image = icon.getImage();
		imageObserver = icon.getImageObserver();
	}
	
	// overload paint()
	public void paint(Graphics g) {
		super.paint(g);
		g.drawImage(image, 0, 0, this.getWidth(), this.getHeight(), this.imageObserver);
	}
}
