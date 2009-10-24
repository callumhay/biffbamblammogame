package bbbleveleditor;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;

import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;

public class BBBLevelEditorMenuBar extends JMenuBar {
	private static final long serialVersionUID = 1L;

	private BBBLevelEditMainWindow levelEditWindow;
	
	// File menu
	private JMenuItem newMenuItem;
	private JMenuItem openMenuItem;
	private JMenuItem closeMenuItem;
	private JMenuItem saveMenuItem;
	private JMenuItem saveAsMenuItem;
	private JMenuItem exitMenuItem;
	
	// Edit menu
	private JMenuItem lvlDimMenuItem;
	private JMenuItem lvlItemsMenuItem;
	
	
	public BBBLevelEditorMenuBar(BBBLevelEditMainWindow window) {
		this.levelEditWindow = window;
		this.addFileMenu();
		this.addEditMenu();
	}
	
	private void addFileMenu() {
		// File menu...
		JMenu fileMenu = new JMenu("File");
		FileMenuActionListener fileMenuActionListener = new FileMenuActionListener();
		
		this.newMenuItem = new JMenuItem("New");
		this.newMenuItem.setActionCommand("new");
		this.newMenuItem.addActionListener(fileMenuActionListener);
		this.newMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
		this.newMenuItem.setEnabled(true);
		
		this.openMenuItem   = new JMenuItem("Open...");
		this.openMenuItem.setActionCommand("open");
		this.openMenuItem.addActionListener(fileMenuActionListener);
		this.openMenuItem.setEnabled(true);
		
		this.closeMenuItem  = new JMenuItem("Close");
		this.closeMenuItem.setActionCommand("close");
		this.closeMenuItem.addActionListener(fileMenuActionListener);
		this.closeMenuItem.setEnabled(false);
		
		this.saveMenuItem   = new JMenuItem("Save");
		this.saveMenuItem.setActionCommand("save");
		this.saveMenuItem.addActionListener(fileMenuActionListener);
		this.saveMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
		this.saveMenuItem.setEnabled(false);
		
		this.saveAsMenuItem = new JMenuItem("Save As...");
		this.saveAsMenuItem.setActionCommand("save_as");
		this.saveAsMenuItem.addActionListener(fileMenuActionListener);
		this.saveAsMenuItem.setEnabled(false);
		
		this.exitMenuItem   = new JMenuItem("Exit");
		this.exitMenuItem.setActionCommand("exit");
		this.exitMenuItem.addActionListener(fileMenuActionListener);
		this.exitMenuItem.setEnabled(true);
		
		fileMenu.add(this.newMenuItem);
		fileMenu.add(this.openMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(this.closeMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(this.saveMenuItem);
		fileMenu.add(this.saveAsMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(this.exitMenuItem);
		
		this.add(fileMenu);
	}
	
	/**
	 * Private class that will interpret action events from the file menu.
	 * @author Callum Hay
	 */
	private class FileMenuActionListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			if (e.getActionCommand().equals("new")) {
				// Initiate the modal dialog for getting info about the new level being created
				NewLevelEditDialog dlg = new NewLevelEditDialog(levelEditWindow);
				dlg.pack();
				dlg.setVisible(true);
				
				// If the user clicked "OK" in the dialog then we create the
				// new level for editing
				if (dlg.getExitedWithOK()) {
					String levelName = dlg.getLevelName();
					int levelWidth 	 = dlg.getLevelWidth();
					int levelHeight  = dlg.getLevelHeight();
					levelEditWindow.addNewLevelEditDocument(levelName, levelWidth, levelHeight);
				}
			}
			else if (e.getActionCommand().equals("open")) {
				levelEditWindow.openLevelEditDocument();
			}
			else if (e.getActionCommand().equals("close")) {
				levelEditWindow.closeLevelEditDocument();
			}
			else if (e.getActionCommand().equals("save")) {
				levelEditWindow.saveLevelEditDocument();
			}	
			else if (e.getActionCommand().equals("save_as")) {
				levelEditWindow.saveAsLevelEditDocument();
			}
			else if (e.getActionCommand().equals("exit")) {
				
			}
		}
	}
	
	private void addEditMenu() {
		// Edit menu...
		JMenu editMenu = new JMenu("Edit");
		EditMenuActionListener editMenuActionListener = new EditMenuActionListener();
		
		this.lvlDimMenuItem   = new JMenuItem("Dimensions...");
		this.lvlDimMenuItem.setActionCommand("dimensions");
		this.lvlDimMenuItem.addActionListener(editMenuActionListener);
		
		this.lvlItemsMenuItem = new JMenuItem("Allowable Item Drops...");
		this.lvlItemsMenuItem.setActionCommand("item_drops");
		this.lvlItemsMenuItem.addActionListener(editMenuActionListener);
		
		editMenu.add(this.lvlDimMenuItem);
		editMenu.add(this.lvlItemsMenuItem);
		
		this.add(editMenu);
	}
	
	/**
	 * Private class that will interpret action events from the edit menu.
	 * @author Callum Hay
	 */
	private class EditMenuActionListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			if (e.getActionCommand().equals("dimensions")) {
				
			}
			else if (e.getActionCommand().equals("item_drops")) {
				
			}
		}
	}
	
	public void UpdateEnabledMenuItems() {
		BBBLevelEditDocumentWindow activeDoc = this.levelEditWindow.getActiveLevelDoc();
		boolean activeDocExists = activeDoc != null;
		
		this.closeMenuItem.setEnabled(activeDocExists);
		this.saveMenuItem.setEnabled(activeDocExists);
		this.saveAsMenuItem.setEnabled(activeDocExists);
		this.lvlDimMenuItem.setEnabled(activeDocExists);
		this.lvlItemsMenuItem.setEnabled(activeDocExists);
	}
}
