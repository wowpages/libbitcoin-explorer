/*
 * Copyright (c) 2011-2014 sx developers (see AUTHORS)
 *
 * This file is part of sx.
 *
 * sx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin.hpp>
#include <sx/utility/console.hpp>

using namespace bc;

bool validsig(transaction_type& tx, size_t input_index,
    elliptic_curve_key& key, const script_type& script_code,
    data_chunk signature)
{
    uint32_t hash_type = 0;
    hash_type = signature.back();
    signature.pop_back();

    hash_digest tx_hash = script_type::generate_signature_hash(
        tx, static_cast<uint32_t>(input_index), script_code, hash_type);
    return key.verify(tx_hash, signature);
}

bool invoke(const int argc, const char* argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: validsig FILENAME N SCRIPT_CODE SIGNATURE" << std::endl;
        return -1;
    }
    const std::string filename = argv[1];
    transaction_type tx;
    if (!load_tx(tx, filename))
        return -1;
    const std::string index_str = argv[2];
    size_t input_index;
    try
    {
        input_index = boost::lexical_cast<size_t>(index_str);
    }
    catch (const boost::bad_lexical_cast&)
    {
        std::cerr << "validsig: Bad N provided." << std::endl;
        return -1;
    }
    const script_type script_code = parse_script(decode_hex(argv[3]));
    if (input_index >= tx.inputs.size())
    {
        std::cerr << "validsig: N out of range." << std::endl;
        return -1;
    }
    const data_chunk signature = decode_hex(argv[4]);
    elliptic_curve_key key;
    if (!read_public_or_private_key(key))
    {
        std::cerr << "Invalid public or private key." << std::endl;
        return -1;
    }
    if (!validsig(tx, input_index, key, script_code, signature))
    {
        std::cout << "Status: Failed" << std::endl;
        return 0;
    }
    std::cout << "Status: OK" << std::endl;
    return 0;
}
