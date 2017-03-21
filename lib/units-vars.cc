# include <mutex>

# include <ah-stl-utils.H>

# include <units-list.H>

# include <json.hpp>

using json = nlohmann::json;

// the following data is declared in units.H
UnitItemTable PhysicalQuantity::tbl;

UnitItemTable Unit::tbl;
//DynSetTree<const Unit*> Unit::unit_tbl;
DynSetHash<const Unit *> Unit::unit_tbl(1000);

static size_t
name_unit_pair_hash(const pair<pair<string, string>, Unit_Convert_Fct_Ptr> & p)
{
  const auto & f = p.first;
  return dft_hash_fct(f.first) + dft_hash_fct(f.second);
}

static size_t fst_unit_pair_hash
(const pair<pair<const Unit*, const Unit*>, Unit_Convert_Fct_Ptr> & p)
{
  return dft_hash_fct(p.first);
}

static size_t snd_unit_pair_hash
(const pair<pair<const Unit*, const Unit*>, Unit_Convert_Fct_Ptr> & p)
{
  return snd_hash_fct(p.first);
}

// UnitHashTbl __unit_name_name_tbl;
// UnitHashTbl __unit_name_symbol_tbl;
// UnitHashTbl __unit_symbol_name_tbl;
// UnitHashTbl __unit_symbol_symbol_tbl;
UnitHashTbl __unit_name_name_tbl(500, name_unit_pair_hash);
UnitHashTbl __unit_name_symbol_tbl(500, name_unit_pair_hash);
UnitHashTbl __unit_symbol_name_tbl(500, name_unit_pair_hash);
UnitHashTbl __unit_symbol_symbol_tbl(500, name_unit_pair_hash);
UnitMap __unit_map(2000, fst_unit_pair_hash, snd_unit_pair_hash);
CompoundUnitTbl __compound_unit_tbl;

//static std::mutex unit_mutex;

const PhysicalQuantity
PhysicalQuantity::null_physical_quantity("NullPhysicalQuantity", "NullPQ",
					 "Null" "Null Physical Quantity");

const Unit Unit::null_unit("NullUnit", "Null Unit", "Null unit", "Null",
			   PhysicalQuantity::null_physical_quantity,
			   numeric_limits<double>::min(),
			   numeric_limits<double>::max());

const double Unit::Invalid_Value = numeric_limits<double>::max();

static const double val_null_quantity = numeric_limits<double>::max();

const VtlQuantity VtlQuantity::null_quantity(Unit::null_unit, val_null_quantity);

bool conversion_exist(const char * src_symbol, const char * tgt_symbol)
{
  //std::lock_guard<std::mutex> critical_section(unit_mutex);
  return exist_conversion(src_symbol, tgt_symbol);
}

double unit_convert(const char * src_symbol, const char * tgt_symbol,
		    double val)
{
  //std::lock_guard<std::mutex> critical_section(unit_mutex);
  return unit_convert_symbol_to_symbol(src_symbol, val, tgt_symbol);
}

static json to_json(const Unit * unit_ptr)
{
  json j;
  j["name"] = unit_ptr->name;
  j["description"] = unit_ptr->description;
  j["symbol"] = unit_ptr->symbol;
  j["latex_symbol"] = unit_ptr->latex_symbol;
  j["minimum_value"] = unit_ptr->min_val;
  j["maximum_value"] = unit_ptr->max_val;
  j["epsilon"] = unit_ptr->get_epsilon();
  return j;
}

static json to_json(const PhysicalQuantity * const pq)
{
  json j;
  j["name"] = pq->name;
  j["description"] = pq->description;
  j["symbol"] = pq->symbol;
  j["latex_symbol"] = pq->latex_symbol;
  j["units"] =
    to_vector(pq->units().maps<json>([] (auto p) { return to_json(p); }));
  
  return j;
}

string units_json()
{
  json j;
  j["Zen_physical_quantities"] =
    to_vector(PhysicalQuantity::quantities().maps<json>([] (auto p)
    { return to_json(p); }));
  return j.dump(2);
}

// The following global singleton variables are generated by extract-cv script
