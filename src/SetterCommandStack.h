/*
 * SetterCommandStack.h is part of Brewtarget, and is Copyright the following
 * authors 2009-2014
 * - A.J. Drobnich <aj.drobnich@gmail.com>
 * - Mik Firestone <mikfire@gmail.com>
 * - Philip Greggory Lee <rocketman768@gmail.com>
 *
 * Brewtarget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Brewtarget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SETTERCOMMANDSTACK_H
#define _SETTERCOMMANDSTACK_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QTimer>
#include <QThread>

// Forward declarations
class SetterCommand;

/*!
 * \class SetterCommandStack
 * \author Philip G. Lee (rocketman768@gmail.com)
 *
 * \brief Collects SetterCommand commands together and periodically executes them in a single transaction.
 *
 * This class is currently unused due to KISS.
 *
 * This is analagous to QUndoStack, except it does not execute commands
 * immediately, but rather collects them together for a certain amount
 * of time, then combines and executes them. Its methods should also
 * be non-blocking and execute in the background thread. I invented it
 * for 2 main reasons: 1) To collect sql write operations into transactions
 * to be more efficient, and 2) to make GUI elements that call
 * BeerXMLElement::set return immediately, offloading the time-consuming
 * sql operations to this thread.
 *
 * NOTE: This class is a bare-bones implementation and needs some
 * fledging-out, for example, popping and undo-ing.
 */
class SetterCommandStack : public QObject
{
   Q_OBJECT
public:
   /*!
    * \param thread is the thread to execute commands from.
    * \param interval_ms is the amount of time between command executions.
    * 100 ms is definitely too long; you can notice the lag visually.
    */
   SetterCommandStack( QThread* thread = QThread::currentThread(), int interval_ms=20 );
   virtual ~SetterCommandStack();
   
   /*!
    * \brief Push a command onto the stack.
    *
    * If the internal timer is not running, this starts a timer. Until the
    * timer runs out, calling push() will aggregate the commands together,
    * and then all aggregated commands run as a single transaction when it
    * times out.
    */
   void push(SetterCommand* command);

   /*!
    * \brief Force the command stack to flush
    */
   void flush();

private slots:
   void executeNext();
   
private:
   // List of the old already-executed commands.
   QList<SetterCommand*> _commands;
   // Max number of old commands to keep in _commands.
   int _commandLimit;
   // Current length of _commands.
   int _numCommands;
   int _executionInterval_ms;
   SetterCommand* _nextCommand;
   SetterCommand* _nextCommandTmp;
   QMutex _commandPtrSwitch;
   QTimer* _timer;
};

#endif /*_SETTERCOMMANDSTACK_H*/
