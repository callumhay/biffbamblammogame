package bbbleveleditor;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

public class BBBLevelEditorMenuBar extends JMenuBar {

	public BBBLevelEditorMenuBar() {
		this.addFileMenu();
		this.addEditMenu();
	}
	
	private void addFileMenu() {
		// File menu...
		JMenu fileMenu = new JMenu("File");
		FileMenuActionListener fileMenuActionListener = new FileMenuActionListener();
		
		JMenuItem newMenuItem    = new JMenuItem("New");
		newMenuItem.setActionCommand("new");
		newMenuItem.addActionListener(fileMenuActionListener);
		
		JMenuItem openMenuItem   = new JMenuItem("Open");
		openMenuItem.setActionCommand("open");
		openMenuItem.addActionListener(fileMenuActionListener);
		
		JMenuItem closeMenuItem  = new JMenuItem("Close");
		closeMenuItem.setActionCommand("close");
		closeMenuItem.addActionListener(fileMenuActionListener);
		
		JMenuItem saveMenuItem   = new JMenuItem("Save");
		saveMenuItem.setActionCommand("save");
		saveMenuItem.addActionListener(fileMenuActionListener);
		
		JMenuItem saveAsMenuItem = new JMenuItem("Save As...");
		saveAsMenuItem.setActionCommand("save_as");
		saveAsMenuItem.addActionListener(fileMenuActionListener);
		
		JMenuItem exitMenuItem   = new JMenuItem("Exit");
		exitMenuItem.setActionCommand("exit");
		exitMenuItem.addActionListener(fileMenuActionListener);		
		
		fileMenu.add(newMenuItem);
		fileMenu.add(openMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(closeMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(saveMenuItem);
		fileMenu.add(saveAsMenuItem);
		fileMenu.addSeparator();
		fileMenu.add(exitMenuItem);
		
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
				
			}
			else if (e.getActionCommand().equals("open")) {
				
			}
			else if (e.getActionCommand().equals("close")) {
				
			}
			else if (e.getActionCommand().equals("save")) {
				
			}	
			else if (e.getActionCommand().equals("save_as")) {
				
			}
			else if (e.getActionCommand().equals("exit")) {
				
			}
		}
	}
	
	private void addEditMenu() {
		// Edit menu...
		JMenu editMenu = new JMenu("Edit");
		EditMenuActionListener editMenuActionListener = new EditMenuActionListener();
		
		JMenuItem lvlDimItem   = new JMenuItem("Dimensions...");
		lvlDimItem.setActionCommand("dimensions");
		lvlDimItem.addActionListener(editMenuActionListener);
		
		JMenuItem lvlThemeItem = new JMenuItem("Theme...");
		lvlThemeItem.setActionCommand("theme");
		lvlThemeItem.addActionListener(editMenuActionListener);
		
		JMenuItem lvlItemsItem = new JMenuItem("Allowable Item Drops...");
		lvlItemsItem.setActionCommand("item_drops");
		lvlItemsItem.addActionListener(editMenuActionListener);
		
		editMenu.add(lvlDimItem);
		editMenu.add(lvlThemeItem);
		editMenu.add(lvlItemsItem);
		
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
			else if (e.getActionCommand().equals("theme")) {
				
			}
			else if (e.getActionCommand().equals("item_drops")) {
				
			}
		}
	}	
}
