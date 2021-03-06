/*
 * InferenceSCM.cc
 *
 * Copyright (C) 2015 OpenCog Foundation
 *
 * Author: Misgana Bayetta <misgana.bayetta@gmail.com>  Sept 2014
 *
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

#ifdef HAVE_GUILE

#include <opencog/guile/SchemeModule.h>

namespace opencog {

class InferenceSCM : public ModuleWrap
{
protected:
	virtual void init();

	Handle do_forward_chaining(Handle h,
	                           Handle rbs,
	                           Handle hfocus_set);

	Handle do_backward_chaining(Handle h,
	                            Handle rbs,
	                            Handle hfocus_set);

	Handle get_rulebase_rules(Handle rbs);

public:
	InferenceSCM();
};

} /*end of namespace opencog*/

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/guile/SchemePrimitive.h>

#include <opencog/rule-engine/forwardchainer/ForwardChainer.h>
#include <opencog/rule-engine/backwardchainer/BackwardChainer.h>

#include "UREConfigReader.h"
using namespace opencog;

InferenceSCM::InferenceSCM() : ModuleWrap("opencog rule-engine") {}

/// This is called while (opencog rule-engine) is the current module.
/// Thus, all the definitions below happen in that module.
void InferenceSCM::init(void)
{
	define_scheme_primitive("cog-fc",
		&InferenceSCM::do_forward_chaining, this, "rule-engine");

	define_scheme_primitive("cog-bc",
		&InferenceSCM::do_backward_chaining, this, "rule-engine");

	define_scheme_primitive("ure-rbs-rules",
		&InferenceSCM::get_rulebase_rules, this, "rule-engine");
}

/**
 * The scheme (cog-fc) function calls this, to perform forward-chaining.
 *
 * @param hsource      The source atom with which to start the chaining.
 * @param rbs          A node, holding the name of the rulebase.
 * @param hfoucs_set   A SetLink containing the atoms to which forward
 *                     chaining will be applied.  If the set link is
 *                     empty, chaining will be invoked on the entire
 *                     atomspace.
 *
 * @return             A SetLink containing the results of FC inference.
 */
Handle InferenceSCM::do_forward_chaining(Handle hsource,
                                         Handle rbs,
                                         Handle hfocus_set)
{
    AtomSpace *as = SchemeSmob::ss_get_env_as("cog-fc");
    HandleSeq focus_set = {};

    if (hfocus_set->getType() == SET_LINK)
        focus_set = hfocus_set->getOutgoingSet();
    else
        throw RuntimeException(
                TRACE_INFO,
                "InferenceSCM::do_forward_chaining - focus set should be SET_LINK type!");

    ForwardChainer fc(*as, rbs, hsource, focus_set);
    fc.do_chain();
    UnorderedHandleSet result = fc.get_chaining_result();

    return as->add_link(SET_LINK, HandleSeq(result.begin(), result.end()));
}

Handle InferenceSCM::do_backward_chaining(Handle h,
                                          Handle rbs,
                                          Handle focus_link)
{
    if (Handle::UNDEFINED == rbs)
        throw RuntimeException(TRACE_INFO,
            "InferenceSCM::do_backward_chaining - invalid rulebase!");

    AtomSpace *as = SchemeSmob::ss_get_env_as("cog-bc");
    BackwardChainer bc(*as, rbs, h,
                       Handle::UNDEFINED /*TODO support vardecl*/,
                       focus_link);

    bc.do_chain();

    return bc.get_results();
}

// XXX FIXME -- this appears to be dead code, that no one uses.
// Can this be removed?
Handle InferenceSCM::get_rulebase_rules(Handle rbs)
{
    if (Handle::UNDEFINED == rbs)
        throw RuntimeException(TRACE_INFO,
            "InferenceSCM::get_rulebase_rules - invalid rulebase!");

    AtomSpace *as = SchemeSmob::ss_get_env_as("ure-rbs-rules");
    UREConfigReader ure_config(*as, rbs);
    auto rules = ure_config.get_rules();
    HandleSeq hs;

    // Copy handles from a rule vector to a handle vector
    for (auto i = rules.begin(); i != rules.end(); i++){
        hs.push_back((*i).get_alias());
    }

    return Handle(createLink(SET_LINK, hs));
}


extern "C" {
void opencog_ruleengine_init(void);
};

void opencog_ruleengine_init(void)
{
    static InferenceSCM inference;
    inference.module_init();
}

#endif // HAVE_GUILE
