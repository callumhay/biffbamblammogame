package bbbleveleditor;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

public class EditDimensionsDialog extends JDialog implements ActionListener {

	private static final long serialVersionUID = -2112006651235185710L;
	private int newWidth;
	private int newHeight;
	
	/*
	private JToggleButton upButton;
	private JToggleButton leftButton;
	private JToggleButton centerButton;
	private JToggleButton rightButton;
	private JToggleButton downButton;
	*/
	
	private JTextField widthTxtField;
	private JTextField heightTxtField;
	
	private JButton okButton;
	private JButton cancelButton;
	private boolean exitWithOK;
	
	public EditDimensionsDialog(JFrame window, int currWidth, int currHeight) {
		super(window, "Level Dimensions", true);
		this.newWidth = currWidth;
		this.newHeight = currHeight;
		this.exitWithOK = false;
		
		/*
		// Setup the orientation buttons - these determine the direction that
		// the dimensions will expand/contract in.
		this.upButton = new JToggleButton(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/up_arrow.png")));
		this.upButton.addActionCommand("up");
		this.leftButton = new JToggleButton(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/left_arrow.png")));
		this.upButton.addActionCommand("left");
		this.centerButton = new JToggleButton("o");
		this.upButton.addActionCommand("center");
		this.rightButton = new JToggleButton(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/right_arrow.png")));
		this.upButton.addActionCommand("right");
		this.downButton = new JToggleButton(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/down_arrow.png"))); 
		this.downButton.addActionCommand("down");
		
		JPanel emptyPanel = new JPanel();
		JPanel orientButtonPanel = new JPanel(new GridLayout(3,3));
		orientButtonPanel.add(emptyPanel);
		orientButtonPanel.add(this.upButton);
		orientButtonPanel.add(emptyPanel);
		orientButtonPanel.add(this.leftButton);
		orientButtonPanel.add(this.centerButton);
		orientButtonPanel.add(this.rightButton);
		orientButtonPanel.add(emptyPanel);
		orientButtonPanel.add(this.downButton);
		orientButtonPanel.add(emptyPanel);		
		*/
		
		JLabel widthLabel = new JLabel("Width:");
		JLabel heightLabel = new JLabel("Height:");
		this.widthTxtField = new JTextField(3);
		this.widthTxtField.setText("" + this.newWidth);
		this.heightTxtField = new JTextField(3);
		this.heightTxtField.setText("" + this.newHeight);
		
		JPanel dimPanel = new JPanel();
		dimPanel.setLayout(new FlowLayout());
		dimPanel.add(widthLabel);
		dimPanel.add(this.widthTxtField);
		dimPanel.add(heightLabel);
		dimPanel.add(this.heightTxtField);
		
		this.setLayout(new BorderLayout());
		this.add(dimPanel, BorderLayout.CENTER);
		
		
		// Setup the ok and cancel buttons
		this.okButton = new JButton("OK");
		this.okButton.setActionCommand("OK");
		this.okButton.addActionListener(this);
		this.cancelButton = new JButton("Cancel");
		this.cancelButton.setActionCommand("Cancel");
		this.cancelButton.addActionListener(this);
		
		JPanel okCancelButtonPanel = new JPanel(new FlowLayout());
		okCancelButtonPanel.add(this.okButton);
		okCancelButtonPanel.add(this.cancelButton);
		
		this.add(okCancelButtonPanel, BorderLayout.SOUTH);
		
		this.setMinimumSize(new Dimension(200, 0));
		this.setResizable(false);
		
		this.setLocationRelativeTo(window);
		int parentWidth = window.getWidth();
		int parentHeight = window.getHeight();
		
		this.setLocation(new Point((parentWidth - this.getWidth()) / 2, (parentHeight - this.getHeight()) / 2));		
		
		this.pack();
	}

	public int getNewWidth() {
		return this.newWidth;
	}
	public int getNewHeight() {
		return this.newHeight;
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("OK")) {
			this.newWidth = Integer.parseInt(this.widthTxtField.getText());
			this.newHeight = Integer.parseInt(this.heightTxtField.getText());
			this.exitWithOK = true;
			this.setVisible(false);
		}
		else if (e.getActionCommand().equals("Cancel")) {
			this.exitWithOK = false;
			this.setVisible(false);
		}
	}
	
	public boolean getExitedWithOK() {
		return this.exitWithOK;
	}
}
