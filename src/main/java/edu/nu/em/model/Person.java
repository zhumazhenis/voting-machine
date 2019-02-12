package edu.nu.em.model;

import javax.persistence.*;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;


@Entity
@Table(name = "person")
public class Person implements Serializable {

  private static final long serialVersionUID = 0x62A6DA99AABDA8A8L;

  private Integer id;
  private String name;
  private List<Skill> skills = new ArrayList<>();

  @Id
  @GeneratedValue
  @Column(name = "id_")
  public Integer getId() {
    return id;
  }

  @OneToMany(mappedBy = "person", fetch = FetchType.LAZY, targetEntity = Skill.class)
  public List<Skill> getSkills() {
    return skills;
  }

  @Column(name = "name_")
  public String getName() {
    return name;
  }

  public void setId(Integer id) {
    this.id = id;
  }

  public void setName(String name) {
    this.name = name;
  }

  public void setSkills(List<Skill> skills) {
    this.skills = skills;
  }
}
