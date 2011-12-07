/*
 * FermentableDialog.cpp is part of Brewtarget, and is Copyright Philip G. Lee
 * (rocketman768@gmail.com), 2009-2011.
 *
 * Brewtarget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Brewtarget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QInputDialog>
#include <QList>
#include "FermentableDialog.h"
#include "FermentableTableModel.h"
#include "database.h"
#include "recipe.h"
#include "MainWindow.h"
#include "fermentable.h"

FermentableDialog::FermentableDialog(MainWindow* parent)
        : QDialog(parent), mainWindow(parent), numFerms(0), fermEdit(new FermentableEditor(this))
{
   setupUi(this);

   connect( pushButton_addToRecipe, SIGNAL( clicked() ), this, SLOT( addFermentable() ) );
   connect( pushButton_edit, SIGNAL( clicked() ), this, SLOT( editSelected() ) );
   connect( pushButton_remove, SIGNAL( clicked() ), this, SLOT( removeFermentable() ) );
   connect( pushButton_new, SIGNAL( clicked() ), this, SLOT( newFermentable() ) );
   connect( fermentableTableWidget, SIGNAL( doubleClicked(const QModelIndex&) ), this, SLOT(addFermentable(const QModelIndex&)) );
   
   connect( &(Database::instance()), SIGNAL(changed(QMetaProperty,QVariant)), this, SLOT(changed(QMetaProperty,QVariant)) );
   
   populateTable();
}

void FermentableDialog::removeFermentable()
{
   QModelIndexList selected = fermentableTableWidget->selectedIndexes();
   QModelIndex translated;
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   translated = fermentableTableWidget->getProxy()->mapToSource(selected[0]);
   Fermentable* ferm = fermentableTableWidget->getModel()->getFermentable(translated.row());
   Database::instance().removeFermentable(ferm);
}

void FermentableDialog::editSelected()
{
   QModelIndexList selected = fermentableTableWidget->selectedIndexes();
   QModelIndex translated;
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   translated = fermentableTableWidget->getProxy()->mapToSource(selected[0]);
   Fermentable* ferm = fermentableTableWidget->getModel()->getFermentable(translated.row());
   fermEdit->setFermentable(ferm);
   fermEdit->show();
}

void FermentableDialog::changed(QMetaProperty prop, QVariant val)
{
   // Notifier should only be the database.
   if( sender() == &(Database::instance()) &&
       prop.propertyIndex() == Database::instance().metaObject().indexOfProperty("fermentables") )
   {
      fermentableTableWidget->getModel()->removeAll();
      populateTable();
   }
}

void FermentableDialog::populateTable()
{
   QList<Fermentable*> ferms;
   Database::instance().getFermentables(ferms);

   numFerms = ferms.length();
   int i;
   for( i = 0; i < numFerms; ++i )
      fermentableTableWidget->getModel()->addFermentable(ferms[i]);
}

void FermentableDialog::addFermentable(const QModelIndex& index)
{
   QModelIndex translated;
   
   // If there is no provided index, get the selected index.
   if( !index.isValid() )
   {
      QModelIndexList selected = fermentableTableWidget->selectedIndexes();
      int row, size, i;

      size = selected.size();
      if( size == 0 )
         return;

      // Make sure only one row is selected.
      row = selected[0].row();
      for( i = 1; i < size; ++i )
      {
         if( selected[i].row() != row )
            return;
      }
      
      translated = fermentableTableWidget->getProxy()->mapToSource(selected[0]);
   }
   else
   {
      // Only respond if the name is selected. Since we connect to double-click signal,
      // this keeps us from adding something to the recipe when we just want to edit
      // one of the other fermentable fields.
      if( index.column() == FERMNAMECOL )
         translated = fermentableTableWidget->getProxy()->mapToSource(index);
      else
         return;
   }
   
   Fermentable *ferm = fermentableTableWidget->getModel()->getFermentable(translated.row());
   
   // TODO: how should we restructure this call?
   mainWindow->addFermentableToRecipe(new Fermentable(*ferm) ); // Need to add a copy so we don't change the database.
}

void FermentableDialog::newFermentable()
{
   QString name = QInputDialog::getText(this, tr("Fermentable name"),
                                          tr("Fermentable name:"));
   if( name.isEmpty() )
      return;
   
   Fermentable* ferm = Database::instance().newFermentable();
   ferm->setName(name);
   fermEdit->setFermentable(ferm);
   fermEdit->show();
}
