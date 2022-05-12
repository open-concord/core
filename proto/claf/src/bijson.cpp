#include "../inc/bijson.hpp"

/** == bijson constructors == */
bijson::bijson() {

}

bijson::bijson(json base) {
  if (base.is_object()) {
    for (auto& [k, v] : base.items()) {
      this->set_key(k, bijson(v));
    }
  } else {
    this->set_base(base);
  }
}

bijson::bijson(bijson a, bijson b) {
  assert(a.type == b.type);
  bijson_type m_type = a.type;
  this->set_type(m_type);

  if (m_type == MAP) {
    for (auto& [ka, va] : a.map_values) {
      auto& va_m = this->map_values[ka];
      va_m = va;
    }
    for (auto& [kb, vb] : b.map_values) {
      if (this->map_values.count(kb) == 0) {this->map_values[kb] = vb;}
      else {
        auto& vb_m = this->map_values[kb];
        if (vb.second > vb_m.second) {vb_m = vb;}
        else if (
            vb.first.type == MAP &&
            vb_m.first.type == MAP &&
            vb.second.get_dir() &&
            vb.second == vb_m.second
        ) {
          vb_m.first = bijson(vb_m.first, vb.first);
        }
      }
    }
  }
}

/** == bijson namespace set == */
void bijson::set_keys(std::map<std::string, bijson> k_v) {
  for (auto& [k, v]: k_v) {
    set_key(k, v);
  }
}
void bijson::set_key(std::string k, bijson v) {
  this->set_type(MAP);
  auto& t_bi = (this->map_values)[k];
  t_bi.second.orient_dir(false); // effectively clear (b4 setting)
  t_bi.second.orient_dir(true);
  t_bi.first = v;
}
void bijson::set_base(json nb) {
  this->set_type(TERMINAL);
  this->base_json = nb;
}
void bijson::set_type(bijson_type nt) {
  this->type = nt;
  if (nt != TERMINAL) {this->base_json = {};}
  if (nt != MAP) {(this->map_values).clear();}
  if (nt != VECT) {(this->vect_values).clear();}
}

/** == bijson namespace clear == */
void bijson::clear_keys(std::vector<std::string> keys) {
  for (auto& k: keys) {
    this->clear_key(k);
  }
}
void bijson::clear_key(std::string k) {
  set_type(MAP);
  auto& t_bi = (this->map_values)[k];
  t_bi.first = bijson();
  t_bi.second.orient_dir(false);
}

/** == misc == */
json bijson::dump() {
  json res;
  if (this->type == MAP) {
    for (auto& [k, v] : this->map_values) {
      if (v.second.get_dir()) {
        res[k] = (v.first).dump();
      }
    }
  } else {
    res = this->base_json;
  }
  return res;
}
