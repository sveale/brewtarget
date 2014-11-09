/*
 * TimeUnitSystem.cpp is part of Brewtarget, and is Copyright the following
 * authors 2009-2014
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

#include "TimeUnitSystem.h"
#include <QStringList>
#include <cmath>
#include "unit.h"

TimeUnitSystem::TimeUnitSystem()
{
   _type = Time;
}

void TimeUnitSystem::loadMap()
{
   scaleToUnit.insert(extrasmall,Units::seconds);
   scaleToUnit.insert(small,Units::minutes);
   scaleToUnit.insert(medium,Units::hours);
   scaleToUnit.insert(large,Units::days);
}

Unit* TimeUnitSystem::unit() { return Units::minutes; };
QString TimeUnitSystem::unitType() { return "entropy"; }
