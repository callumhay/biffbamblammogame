package bbbleveleditor;

import java.awt.event.*;
import javax.swing.*;

import bbbleveleditor.BBBLevelEditMainWindow.ToolMode;

public class BBBLevelEditorMenuBar extends JMenuBar {
	private static final long serialVersionUID = 1L;

	private BBBLevelEditMainWindow levelEditWindow;
	
	// File menu
	private JMenu fileMenu				= null;
	private JMenuItem newMenuItem		= null;
	private JMenuItem openMenuItem		= null;
	private JMenuItem closeMenuItem		= null;
	private JMenuItem saveMenuItem		= null;
	private JMenuItem saveAsMenuItem	= null;
	private JMenuItem exitMenuItem		= null;
	
	// Edit menu
	private JMenu editMenu				= null;
	private JMenuItem lvlDimMenuItem	= null;
	private JMenuItem lvlItemsMenuItem	= null;
	private JMenuItem lvlProperties     = null;
	//private JMenuItem editBlock			= null;
	//private JMenuItem portalsMenuItem   = null;
	
	// Tools menu
	private JMenu toolsMenu								= null;
	private JRadioButtonMenuItem paintBrushMenuItem		= null;
	private JRadioButtonMenuItem  eraserMenuItem		= null;
	
	
	public BBBLevelEditorMenuBar(BBBLevelEditMainWindow window) {
		this.levelEditWindow = window;
		this.addFileMenu();
		this.addEditMenu();
		this.addToolsMenu();
	}
	
	private void addFileMenu() {
		// File menu...
		this.fileMenu = new JMenu("File");
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
		
		this.fileMenu.add(this.newMenuItem);
		this.fileMenu.add(this.openMenuItem);
		this.fileMenu.addSeparator();
		this.fileMenu.add(this.closeMenuItem);
		this.fileMenu.addSeparator();
		this.fileMenu.add(this.saveMenuItem);
		this.fileMenu.add(this.saveAsMenuItem);
		this.fileMenu.addSeparator();
		this.fileMenu.add(this.exitMenuItem);
		
		this.add(this.fileMenu);
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
		this.editMenu = new JMenu("Edit");
		EditMenuActionListener editMenuActionListener = new EditMenuActionListener();
		
		this.lvlDimMenuItem   = new JMenuItem("Dimensions...");
		this.lvlDimMenuItem.setActionCommand("dimensions");
		this.lvlDimMenuItem.addActionListener(editMenuActionListener);
		this.lvlDimMenuItem.setEnabled(false);
		
		this.lvlItemsMenuItem = new JMenuItem("Item Drops...");
		this.lvlItemsMenuItem.setActionCommand("item_drops");
		this.lvlItemsMenuItem.addActionListener(editMenuActionListener);
		this.lvlItemsMenuItem.setEnabled(false);
		
		this.lvlProperties = new JMenuItem("Level Properties...");
		this.lvlProperties.setActionCommand("lvl_properties");
		this.lvlProperties.addActionListener(editMenuActionListener);
		this.lvlProperties.setEnabled(false);
		
		//this.editBlock = new JMenuItem("Block Properties...");
		//this.editBlock.setActionCommand("block_properties");
		//this.editBlock.addActionListener(editMenuActionListener);
		//this.editBlock.setEnabled(false);
		
		//this.portalsMenuItem  = new JMenuItem("Portals...");
		//this.portalsMenuItem.setActionCommand("portals");
		//this.portalsMenuItem.addActionListener(editMenuActionListener);
		//this.portalsMenuItem.setEnabled(false);
		
		this.editMenu.add(this.lvlDimMenuItem);
		this.editMenu.add(this.lvlItemsMenuItem);
		this.editMenu.add(this.lvlProperties);
		//this.editMenu.add(this.portalsMenuItem);
		this.editMenu.setEnabled(false);
		
		this.add(this.editMenu);
	}
	
	private void addToolsMenu() {
		this.toolsMenu = new JMenu("Tools");
		ToolsMenuActionListener toolsMenuActionListener = new ToolsMenuActionListener();
		
		ImageIcon paintBrushIcon = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/paintbrush_icon_16x16.png"));
		this.paintBrushMenuItem = new JRadioButtonMenuItem("Paint Brush", paintBrushIcon);
		this.paintBrushMenuItem.setActionCommand("paint");
		this.paintBrushMenuItem.addActionListener(toolsMenuActionListener);
		this.paintBrushMenuItem.setEnabled(true);
		this.paintBrushMenuItem.setAccelerator(KeyStroke.getKeyStroke('b'));
		
		ImageIcon eraserIcon = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/eraser_icon_16x16.png"));
		this.eraserMenuItem = new JRadioButtonMenuItem("Eraser", eraserIcon);
		this.eraserMenuItem.setActionCommand("eraser");
		this.eraserMenuItem.addActionListener(toolsMenuActionListener);
		this.eraserMenuItem.setEnabled(true);
		this.eraserMenuItem.setAccelerator(KeyStroke.getKeyStroke('e'));
		
		ButtonGroup group = new ButtonGroup();
	    group.add(this.paintBrushMenuItem);
	    group.add(this.eraserMenuItem);
		
	    this.toolsMenu.add(this.paintBrushMenuItem);
	    this.toolsMenu.add(this.eraserMenuItem);
	    
		this.add(this.toolsMenu);
	}
	
	/**
	 * Private class that will interpret action events from the edit menu.
	 * @author Callum Hay
	 */
	private class EditMenuActionListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			BBBLevelEditDocumentWindow activeDoc = levelEditWindow.getActiveLevelDoc();
			if (activeDoc == null) {
				assert(false);
				return;
			}
			
			if (e.getActionCommand().equals("dimensions")) {
				EditDimensionsDialog dlg = new EditDimensionsDialog(levelEditWindow, 
						activeDoc.GetLevelWidth(), activeDoc.GetLevelHeight());
				dlg.setVisible(true);
				
				if (dlg.getExitedWithOK()) {
					activeDoc.setLevelDimensions(dlg.getNewWidth(), dlg.getNewHeight());
				}
			}
			else if (e.getActionCommand().equals("item_drops")) {
				EditItemDropsDialog dlg = new EditItemDropsDialog(levelEditWindow);
				dlg.setItemDropSettings(activeDoc.getItemDropSettings());
				dlg.setVisible(true);
				
				if (dlg.getExitedWithOK()) {
					activeDoc.setItemDropSettings(dlg.getItemDropSettings());
				}
			}
			else if (e.getActionCommand().equals("lvl_properties")) {
				EditLevelPropertiesDialog dlg = new EditLevelPropertiesDialog(levelEditWindow, activeDoc);
				dlg.setVisible(true);
			}
			
			//else if (e.getActionCommand().equals("block_properties")) {
				
			//}
			/*
			else if (e.getActionCommand().equals("portals")) {
				BBBLevelEditDocumentWindow activeDoc = levelEditWindow.getActiveLevelDoc();
				if (activeDoc == null) {
					assert(false);
					return;
				}
				
				PortalDialog dlg = new PortalDialog(levelEditWindow, activeDoc);
				dlg.setVisible(true);
				// TODO ...
				
			}
			*/
		}
	}
	
	private class ToolsMenuActionListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			if (e.getActionCommand().equals("paint")) {
				levelEditWindow.setToolMode(BBBLevelEditMainWindow.ToolMode.PAINT);
			}
			else if (e.getActionCommand().equals("eraser")) {
				levelEditWindow.setToolMode(BBBLevelEditMainWindow.ToolMode.ERASE);
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
		this.lvlProperties.setEnabled(activeDocExists);
		
		boolean editMenuEnabled = this.lvlDimMenuItem.isEnabled() || this.lvlItemsMenuItem.isEnabled();
		this.editMenu.setEnabled(editMenuEnabled);
	}
	
	public void updateToolMode(ToolMode mode) {
		if (mode == BBBLevelEditMainWindow.ToolMode.PAINT && this.paintBrushMenuItem != null) {
			this.paintBrushMenuItem.setSelected(true);
		}
		else if (mode == BBBLevelEditMainWindow.ToolMode.ERASE && this.eraserMenuItem != null) {
			this.eraserMenuItem.setSelected(true);
		}
	}
}
