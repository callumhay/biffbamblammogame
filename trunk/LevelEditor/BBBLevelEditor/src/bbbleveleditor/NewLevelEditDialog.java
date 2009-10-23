package bbbleveleditor;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.text.ParseException;

import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFormattedTextField;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextField;
import javax.swing.text.MaskFormatter;

public class NewLevelEditDialog extends JDialog implements PropertyChangeListener {

	private JTextField levelNameTxtField;
	private JTextField levelWidthTxtField;
	private JTextField levelHeightTxtField;
	private JOptionPane optionPane;
	
	private boolean exitWithOK;
	
	public static int DEFAULT_LEVEL_WIDTH 	= 20;
	public static int DEFAULT_LEVEL_HEIGHT 	= 40;
	
	public NewLevelEditDialog(JFrame window) {
		super(window, "New Level", true);
		this.exitWithOK = false;
		
		this.levelNameTxtField = new JTextField("new_level");
		this.levelWidthTxtField  = new JTextField("" + DEFAULT_LEVEL_WIDTH);
		this.levelWidthTxtField.setColumns(3);
		this.levelHeightTxtField = new JTextField("" + DEFAULT_LEVEL_HEIGHT);
		this.levelHeightTxtField.setColumns(3);

		Object[] optionPaneItems = { new JLabel("Level:"), this.levelNameTxtField,
									 new JLabel("Width:"), this.levelWidthTxtField, 
									 new JLabel("Height:"), this.levelHeightTxtField };
		
		this.optionPane = new JOptionPane(optionPaneItems, JOptionPane.PLAIN_MESSAGE, JOptionPane.OK_CANCEL_OPTION, null);
		this.optionPane.addPropertyChangeListener(this);
		this.setContentPane(this.optionPane);
		
		this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
	}

	@Override
	public void propertyChange(PropertyChangeEvent e) {
		if (e.getSource() == this.optionPane) {
			Object optionPaneValue = this.optionPane.getValue();
			if (optionPaneValue == null) {
				return;
			}
			if (optionPaneValue == JOptionPane.UNINITIALIZED_VALUE) {
                return;
            }
			
			this.optionPane.setValue(JOptionPane.UNINITIALIZED_VALUE);
			
			if (optionPaneValue.equals(JOptionPane.OK_OPTION)) {
				if (this.levelNameTxtField.getText().isEmpty()) {
					this.levelHeightTxtField.setText("new_level");
				}
				this.exitWithOK = true;
				this.setVisible(false);
			}
			else if (optionPaneValue.equals(JOptionPane.CANCEL_OPTION)) {
				this.exitWithOK = false;
				this.setVisible(false);	
			}
		}
	}
	
	public String getLevelName() {
		return this.levelNameTxtField.getText();
	}
	public int getLevelWidth() {
		return Integer.parseInt(this.levelWidthTxtField.getText());
	}
	public int getLevelHeight() {
		return Integer.parseInt(this.levelHeightTxtField.getText());
	}
	public boolean getExitedWithOK() {
		return this.exitWithOK;
	}
	
}
