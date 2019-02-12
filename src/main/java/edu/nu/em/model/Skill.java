package edu.nu.em.model;

import javax.persistence.*;
import java.io.Serializable;


@Entity
@Table(name = "skill")
public class Skill implements Serializable {

  private static final long serialVersionUID = 0x63A6DA99AABDA8A8L;

  private Integer id;
  private String name;
  private Person person;

  @Id
  @GeneratedValue
  @Column(name = "id_")
  public Integer getId() {
    return id;
  }

  @Column(name = "name_")
  public String getName() {
    return name;
  }

  @ManyToOne(fetch = FetchType.EAGER, targetEntity = Person.class, cascade = CascadeType.ALL)
  @JoinColumn(name = "person_id_")
  public Person getPerson() {
    return person;
  }

  public void setId(Integer id) {
    this.id = id;
  }

  public void setName(String name) {
    this.name = name;
  }

  public void setPerson(Person person) {
    this.person = person;
  }
}
