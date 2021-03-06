/*
 * FUNCTION:
 * Persistent Atom storage base class.
 *
 * Copyright (c) 2008,2009,2013 Linas Vepstas <linas@linas.org>
 *
 * LICENSE:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/persist/sql/AtomStorage.h>

using namespace opencog;

AtomStorage::AtomStorage()
{
    // Nothing to do in the base class.
}


AtomStorage::~AtomStorage()
{
    // Nothing to do in the base class.
}

void AtomStorage::storeAtomSpace(AtomSpace* atomspace)
{ 
    store(atomspace->get_atomtable());
}

void AtomStorage::loadAtomSpace(AtomSpace* atomspace)
{
    load(atomspace->get_atomtable());
}

void AtomStorage::clearAndLoadAtomSpace(AtomSpace* atomspace)
{ 
    atomspace->clear(); 
    load(atomspace->get_atomtable());
}


/* ============================= END OF FILE ================= */
