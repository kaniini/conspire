/* Conspire
 * Copyright (C) 2008 William Pitcock
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <glib.h>

/* outside of the tree, use <conspire/foo.h> instead. */
#include "common/plugin.h"
#include "common/xchat.h"
#include "common/command_factory.h"
#include "common/outbound.h"
#include "common/util.h"
#include "common/text.h"

typedef struct
{
    char *code, *country;
} domain_t;

static int
country_compare (const void *a, const void *b)
{
    return strcasecmp (a, ((domain_t *)b)->code);
}

static const domain_t domain[] =
{
    {"AC", N_("Ascension Island")},
    {"AD", N_("Andorra")},
    {"AE", N_("United Arab Emirates")},
    {"AERO", N_("Air-transport industry")},
    {"AF", N_("Afghanistan")},
    {"AG", N_("Antigua and Barbuda")},
    {"AI", N_("Anguilla")},
    {"AL", N_("Albania")},
    {"AM", N_("Armenia")},
    {"AN", N_("Netherlands Antilles")},
    {"AO", N_("Angola")},
    {"AQ", N_("Antarctica")},
    {"AR", N_("Argentina")},
    {"AS", N_("American Samoa")},
    {"ASIA", N_("Pan-Asia and Asia Pacific")},
    {"AT", N_("Austria")},
    {"AU", N_("Australia")},
    {"AW", N_("Aruba")},
    {"AX", N_("Aland Islands")},
    {"AZ", N_("Azerbaijan")},
    {"BA", N_("Bosnia and Herzegovina")},
    {"BB", N_("Barbados")},
    {"BD", N_("Bangladesh")},
    {"BE", N_("Belgium")},
    {"BF", N_("Burkina Faso")},
    {"BG", N_("Bulgaria")},
    {"BH", N_("Bahrain")},
    {"BI", N_("Burundi")},
    {"BIZ", N_("Business")},
    {"BJ", N_("Benin")},
    {"BL", N_("Saint Barthelemy")},
    {"BM", N_("Bermuda")},
    {"BN", N_("Brunei Darussalam")},
    {"BO", N_("Bolivia")},
    {"BR", N_("Brazil")},
    {"BS", N_("Bahamas")},
    {"BT", N_("Bhutan")},
    {"BV", N_("Bouvet Island")},
    {"BW", N_("Botswana")},
    {"BY", N_("Belarus")},
    {"BZ", N_("Belize")},
    {"CA", N_("Canada")},
    {"CAT", N_("Catalan linguistic and cultural community")},
    {"CC", N_("Cocos (Keeling) Islands")},
    {"CD", N_("The Democratic Republic of the Congo")},
    {"CF", N_("Central African Republic")},
    {"CG", N_("Congo")},
    {"CH", N_("Switzerland")},
    {"CI", N_("Cote d'Ivoire")},
    {"CK", N_("Cook Islands")},
    {"CL", N_("Chile")},
    {"CM", N_("Cameroon")},
    {"CN", N_("China")},
    {"CO", N_("Colombia")},
    {"COM", N_("Commercial")},
    {"COOP", N_("Cooperative associations")},
    {"CR", N_("Costa Rica")},
    {"CU", N_("Cuba")},
    {"CV", N_("Cape Verde")},
    {"CX", N_("Christmas Island")},
    {"CY", N_("Cyprus")},
    {"CZ", N_("Czech Republic")},
    {"DE", N_("Germany")},
    {"DJ", N_("Djibouti")},
    {"DK", N_("Denmark")},
    {"DM", N_("Dominica")},
    {"DO", N_("Dominican Republic")},
    {"DZ", N_("Algeria")},
    {"EC", N_("Ecuador")},
    {"EDU", N_("Colleges & universities")},
    {"EE", N_("Estonia")},
    {"EG", N_("Egypt")},
    {"EH", N_("Western Sahara")},
    {"ER", N_("Eritrea")},
    {"ES", N_("Spain")},
    {"ET", N_("Ethiopia")},
    {"EU", N_("European Union")},
    {"FI", N_("Finland")},
    {"FJ", N_("Fiji")},
    {"FK", N_("Falkland Islands (Malvinas)")},
    {"FM", N_("Federated States of Micronesia")},
    {"FO", N_("Faroe Islands")},
    {"FR", N_("France")},
    {"GA", N_("Gabon")},
    {"GB", N_("United Kingdom")},
    {"GD", N_("Grenada")},
    {"GE", N_("Georgia")},
    {"GF", N_("French Guiana")},
    {"GG", N_("Guernsey")},
    {"GH", N_("Ghana")},
    {"GI", N_("Gibraltar")},
    {"GL", N_("Greenland")},
    {"GM", N_("Gambia")},
    {"GN", N_("Guinea")},
    {"GOV", N_("United States Government")},
    {"GP", N_("Guadeloupe")},
    {"GQ", N_("Equatorial Guinea")},
    {"GR", N_("Greece")},
    {"GS", N_("South Georgia and the South Sandwich Islands")},
    {"GT", N_("Guatemala")},
    {"GU", N_("Guam")},
    {"GW", N_("Guinea-Bissau")},
    {"GY", N_("Guyana")},
    {"HK", N_("Hong Kong")},
    {"HM", N_("Heard Island and McDonald Islands")},
    {"HN", N_("Honduras")},
    {"HR", N_("Croatia")},
    {"HT", N_("Haiti")},
    {"HU", N_("Hungary")},
    {"ID", N_("Indonesia")},
    {"IE", N_("Ireland")},
    {"IL", N_("Israel")},
    {"IM", N_("Isle of Man")},
    {"IN", N_("India")},
    {"INFO", N_("Informational")},
    {"INT", N_("International")},
    {"IO", N_("British Indian Ocean Territory")},
    {"IQ", N_("Iraq")},
    {"IR", N_("Islamic Republic of Iran")},
    {"IS", N_("Iceland")},
    {"IT", N_("Italy")},
    {"JE", N_("Jersey")},
    {"JM", N_("Jamaica")},
    {"JO", N_("Jordan")},
    {"JOBS", N_("Human resources")},
    {"JP", N_("Japan")},
    {"KE", N_("Kenya")},
    {"KG", N_("Kyrgyzstan")},
    {"KH", N_("Cambodia")},
    {"KI", N_("Kiribati")},
    {"KM", N_("Comoros")},
    {"KN", N_("Saint Kitts and Nevis")},
    {"KP", N_("Democratic People's Republic of Korea")},
    {"KR", N_("Republic of Korea")},
    {"KW", N_("Kuwait")},
    {"KY", N_("Cayman Islands")},
    {"KZ", N_("Kazakhstan")},
    {"LA", N_("Lao People's Democratic Republic")},
    {"LB", N_("Lebanon")},
    {"LC", N_("Saint Lucia")},
    {"LI", N_("Liechtenstein")},
    {"LK", N_("Sri Lanka")},
    {"LR", N_("Liberia")},
    {"LS", N_("Lesotho")},
    {"LT", N_("Lithuania")},
    {"LU", N_("Luxembourg")},
    {"LV", N_("Latvia")},
    {"LY", N_("Libyan Arab Jamahiriya")},
    {"MA", N_("Morocco")},
    {"MC", N_("Monaco")},
    {"MD", N_("Republic of Moldova")},
    {"ME", N_("Montenegro")},
    {"MF", N_("Saint Martin")},
    {"MG", N_("Madagascar")},
    {"MH", N_("Marshall Islands")},
    {"MIL", N_("United States Military")},
    {"MK", N_("The Former Yugoslav Republic of Macedonia")},
    {"ML", N_("Mali")},
    {"MM", N_("Myanmar")},
    {"MN", N_("Mongolia")},
    {"MO", N_("Macao")},
    {"MOBI", N_("Mobile devices")},
    {"MP", N_("Northern Mariana Islands")},
    {"MQ", N_("Martinique")},
    {"MR", N_("Mauritania")},
    {"MS", N_("Montserrat")},
    {"MT", N_("Malta")},
    {"MU", N_("Mauritius")},
    {"MUSEUM", N_("Museums")},
    {"MV", N_("Maldives")},
    {"MW", N_("Malawi")},
    {"MX", N_("Mexico")},
    {"MY", N_("Malaysia")},
    {"MZ", N_("Mozambique")},
    {"NA", N_("Namibia")},
    {"NAME", N_("Individuals")},
    {"NC", N_("New Caledonia")},
    {"NE", N_("Niger")},
    {"NET", N_("Networks")},
    {"NF", N_("Norfolk Island")},
    {"NG", N_("Nigeria")},
    {"NI", N_("Nicaragua")},
    {"NL", N_("Netherlands")},
    {"NO", N_("Norway")},
    {"NP", N_("Nepal")},
    {"NR", N_("Nauru")},
    {"NU", N_("Niue")},
    {"NZ", N_("New Zealand")},
    {"OM", N_("Oman")},
    {"ORG", N_("Organizations")},
    {"PA", N_("Panama")},
    {"PE", N_("Peru")},
    {"PF", N_("French Polynesia")},
    {"PG", N_("Papua New Guinea")},
    {"PH", N_("Philippines")},
    {"PK", N_("Pakistan")},
    {"PL", N_("Poland")},
    {"PM", N_("Saint Pierre and Miquelon")},
    {"PN", N_("Pitcairn")},
    {"PR", N_("Puerto Rico")},
    {"PRO", N_("Credentialed professionals")},
    {"PS", N_("Palestinian Territory, Occupied")},
    {"PT", N_("Portugal")},
    {"PW", N_("Palau")},
    {"PY", N_("Paraguay")},
    {"QA", N_("Qatar")},
    {"RE", N_("Reunion")},
    {"RO", N_("Romania")},
    {"RS", N_("Serbia")},
    {"RU", N_("Russian Federation")},
    {"RW", N_("Rwanda")},
    {"SA", N_("Saudi Arabia")},
    {"SB", N_("Solomon Islands")},
    {"SC", N_("Seychelles")},
    {"SD", N_("Sudan")},
    {"SE", N_("Sweden")},
    {"SG", N_("Singapore")},
    {"SH", N_("Saint Helena")},
    {"SI", N_("Slovenia")},
    {"SJ", N_("Svalbard and Jan Mayen")},
    {"SK", N_("Slovakia")},
    {"SL", N_("Sierra Leone")},
    {"SM", N_("San Marino")},
    {"SN", N_("Senegal")},
    {"SO", N_("Somalia")},
    {"SR", N_("Suriname")},
    {"ST", N_("Sao Tome and Principe")},
    {"SU", N_("Soviet Union (being phased out)")},
    {"SV", N_("El Salvador")},
    {"SY", N_("Syrian Arab Republic")},
    {"SZ", N_("Swaziland")},
    {"TC", N_("Turks and Caicos Islands")},
    {"TD", N_("Chad")},
    {"TEL", N_("Contact data")},
    {"TF", N_("French Southern Territories")},
    {"TG", N_("Togo")},
    {"TH", N_("Thailand")},
    {"TJ", N_("Tajikistan")},
    {"TK", N_("Tokelau")},
    {"TL", N_("Timor-Leste")},
    {"TM", N_("Turkmenistan")},
    {"TN", N_("Tunisia")},
    {"TO", N_("Tonga")},
    {"TP", N_("Portuguese Timor (being phased out)")},
    {"TR", N_("Turkey")},
    {"TRAVEL", N_("Travel industry")},
    {"TT", N_("Trinidad and Tobago")},
    {"TV", N_("Tuvalu")},
    {"TW", N_("Taiwan")},
    {"TZ", N_("United Republic ofTanzania")},
    {"UA", N_("Ukraine")},
    {"UG", N_("Uganda")},
    {"UK", N_("United Kingdom")},
    {"UM", N_("United States Minor Outlying Islands")},
    {"US", N_("United States")},
    {"UY", N_("Uruguay")},
    {"UZ", N_("Uzbekistan")},
    {"VA", N_("Holy See (Vatican City State)")},
    {"VC", N_("Saint Vincent and the Grenadines")},
    {"VE", N_("Bolivarian Republic of Venezuela")},
    {"VG", N_("Virgin Islands, British")},
    {"VI", N_("Virgin Islands, U.S.")},
    {"VN", N_("Vietnam")},
    {"VU", N_("Vanuatu")},
    {"WF", N_("Wallis and Futuna")},
    {"WS", N_("Samoa")},
    {"YE", N_("Yemen")},
    {"YT", N_("Mayotte")},
    {"YU", N_("Yugoslavia (being phased out)")},
    {"ZA", N_("South Africa")},
    {"ZM", N_("Zambia")},
    {"ZW", N_("Zimbabwe")},
};

gchar *
country (gchar *hostname)
{
    gchar *p;
    domain_t *dom;

    if (!hostname || !*hostname || isdigit ((guchar) hostname[strlen (hostname) - 1]))
        return _("Unknown");
    if ((p = strrchr (hostname, '.')))
        p++;
    else
        p = hostname;

    dom = bsearch (p, domain, sizeof (domain) / sizeof (domain_t),
            sizeof (domain_t), country_compare);

    if (!dom)
        return _("Unknown");

    return _(dom->country);
}

void
country_search (gchar *pattern, void *ud, void (*print)(void *, gchar *, ...))
{
    const domain_t *dom;
    gint i;

    for (i = 0; i < sizeof (domain) / sizeof (domain_t); i++)
    {
        dom = &domain[i];
        if (match (pattern, dom->country) || match (pattern, _(dom->country)))
        {
            print (ud, "%s = %s\n", dom->code, _(dom->country));
        }
    }
}

CommandResult
cmd_country (struct session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
    gchar *code = word[2];
    if (*code)
    {
        /* search? */
        if (strcmp (code, "-s") == 0)
        {
            country_search (word[3], sess, (void *)PrintTextf);
            return CMD_EXEC_OK;
        }

        /* search, but forgot the -s */
        if (strchr (code, '*'))
        {
            country_search (code, sess, (void *)PrintTextf);
            return CMD_EXEC_OK;
        }

        sprintf (tbuf, "%s = %s\n", code, country (code));
        PrintText (sess, tbuf);
        return CMD_EXEC_OK;
    }
    return CMD_EXEC_FAIL;
}

gboolean
init(Plugin *p)
{
    command_register("COUNTRY", "COUNTRY [-s] <code|wildcard>, finds a country code", 0, cmd_country);
    return TRUE;
}

gboolean
fini(Plugin *p)
{
    command_remove_handler("COUNTRY", cmd_country);
    return TRUE;
}

PLUGIN_DECLARE("Country Codes", PACKAGE_VERSION,
    "Finds a country code (because Google is too slow).",
    "Kiyoshi Aman", init, fini);
